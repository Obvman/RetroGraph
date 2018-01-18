#include "../headers/AnimationState.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>

#include "../headers/units.h"
#include "../headers/utils.h"
#include "../headers/UserSettings.h"

namespace rg {

AnimationState::AnimationState() : 
        m_particles{ },
        m_animationFPS{ std::get<uint32_t>(UserSettings::inst().getVal("Widgets-Main.FPS")) } {

    srand(static_cast<uint32_t>(time(nullptr)));
    for (auto i = size_t{ 0U }; i < numParticles; ++i) {
        m_particles.emplace_back();
    }
}


AnimationState::~AnimationState() {
}

void AnimationState::drawParticles() const {
    int checks = 0;
    for (const auto& p : m_particles) {
        p.draw();

        auto nextX = uint32_t{ p.cellX + 1 };
        auto nextY = uint32_t{ p.cellY + 1};
        auto prevY = int32_t{ static_cast<int32_t>(p.cellY) - 1}; // can be negative
        if (nextX >= numCellsPerSide)
            nextX = 0;
        if (nextY >= numCellsPerSide)
            nextY = 0;
        if (prevY < 0)
            prevY = numCellsPerSide - 1;

        const auto cellsToCheck = {
            &(m_cells[p.cellX][p.cellY]),
            &(m_cells[p.cellX][nextY]),
            &(m_cells[nextX][p.cellY]),
            &(m_cells[nextX][prevY]),
            &(m_cells[nextX][nextY]),
        };

        for (const auto cell : cellsToCheck) {
            for (const auto neighbour : *cell) {
                if (&p == neighbour) continue;

                checks++;
                const auto dx{ fabs(p.x - neighbour->x) };
                const auto dy{ fabs(p.y - neighbour->y) };
                constexpr auto radiusSq{ particleConnectionDistance * particleConnectionDistance };
                const auto distance{ dx * dx + dy * dy };

                if (distance < radiusSq) {
                    const float distFactor{ 1.0f - distance / radiusSq };
                    glColor4f(1.0f, 1.0f, 1.0f, distFactor);
                    glBegin(GL_LINES);
                    {
                        glVertex2f(p.x, p.y);
                        glVertex2f(neighbour->x, neighbour->y);
                    } glEnd();
                }
            }
        }
    }
    printf("Checks: %d\n", checks);
}

void AnimationState::update(uint32_t ticks) {
    if (ticks % (ticksPerSecond / m_animationFPS) == 0) {
        using clock = std::chrono::high_resolution_clock;
        static auto time_start = clock::now();

        auto time_end = clock::now();
        const auto deltaTimeStep{ time_end - time_start };
        time_start = clock::now();
        auto dt{ std::chrono::duration_cast<std::chrono::duration<float>>(deltaTimeStep).count() };

        // If a lot of time has passed, set dt to a small value so we don't have
        // one large jump
        if (dt > 1.0f) {
            dt = 0.016f;
        }

        printTimeToExecuteHighRes("Update", [this, dt]() {
            for (auto& p : m_particles) {
                p.update(dt);
            }

            updateSpacialPartitioningGrid();
        });
    }
}

void AnimationState::updateSpacialPartitioningGrid() {
    // Reset the particle list for each cell
    for (auto& x : m_cells) {
        for (auto& y : x) {
            y.clear();
        }
    }

    for (auto& p : m_particles) {
        // Get the cell index from the particle's position. offset the particle
        // coord by 1.0f to give value in range 0 .. numCellsPerSide - 1
        const uint32_t cellX{ static_cast<uint32_t>((p.x + 1.0f) / cellSize) };
        const uint32_t cellY{ static_cast<uint32_t>((p.y + 1.0f) / cellSize) };
        if (cellX < 0 || cellY < 0 || cellX >= numCellsPerSide || cellY >= numCellsPerSide) {
            printf("(%f, %f) -> %d, %d\n", p.x, p.y, cellX, cellY);
            assert(cellX >= 0);
            assert(cellY >= 0);
            assert(cellX < numCellsPerSide);
            assert(cellY < numCellsPerSide);
        }

        p.cellX = cellX;
        p.cellY = cellY;

        m_cells[cellX][cellY].push_back(&p);
    }
}

Particle::Particle() {
    constexpr auto seed = 10U;

    // Randomly initialise each particle
    x = particleMinPos + static_cast<float> (rand()) /(static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos)));
    y = particleMinPos + static_cast<float> (rand()) /(static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos)));
    dirX = particleMinPos + static_cast<float> (rand()) /(static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos)));
    dirY = particleMinPos + static_cast<float> (rand()) /(static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos)));
    size = particleMinSize + static_cast<float> (rand()) /(static_cast<float>(RAND_MAX/(particleMaxSize-particleMinSize)));
    speed = particleMinSpeed + static_cast<float> (rand()) /(static_cast<float>(RAND_MAX/(particleMaxSpeed-particleMinSpeed)));
}

Particle::Particle(float x_, float y_, float dirX_, float dirY_, float size_) :
    x{ x_ }, y{ y_ },
    dirX{ dirX_ }, dirY{ dirY_ },
    size{ size_ }, speed{ 0.01f } {


}

void Particle::update(float dt) {
    x += speed * dirX * dt;
    y += speed * dirY * dt;

    // If we move off the screen, wrap the particle around to the other side
    if (x <= -1.0f) {
        x = particleMaxPos;
        y = -y;
    } else if (x >= 1.0f) {
        x = particleMinPos;
        y = -y;
    } 
    if (y <= -1.0f) {
        x = -x;
        y = particleMaxPos;
    } else if (y >= 1.0f) {
        x = -x;
        y = particleMinPos;
    }
}

void Particle::draw() const {
    constexpr auto circleLines = int32_t{ 10 };
    glPushMatrix(); {
        glTranslatef(x, y, 0.0f);

        glColor4f(PARTICLE_R, PARTICLE_G, PARTICLE_B, PARTICLE_A);
        if (size > particleMinSize * 2) {
            glBegin(GL_TRIANGLE_FAN); {
                glVertex2f(0.0f, 0.0f);
                for (int i = 0; i < circleLines; ++i) {
                    const auto theta{ 2.0f * 3.1415926f * static_cast<float>(i) /
                        static_cast<float>(circleLines - 1) };
                    glVertex2f( size * cosf(theta) ,  size * sinf(theta) );
                }
            } glEnd();
        } else {
            glBegin(GL_QUADS); {
                glVertex2f(-size/2.0f, size/2.0f);
                glVertex2f(size/2.0f, size/2.0f);
                glVertex2f(size/2.0f, -size/2.0f);
                glVertex2f(-size/2.0f, -size/2.0f);
            } glEnd();
        }

    } glPopMatrix();
}

}