#include "../headers/AnimationState.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <algorithm>
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

    // Generate particles and populate the cell particle observer lists.
    srand(static_cast<uint32_t>(time(nullptr)));
    for (auto i = size_t{ 0U }; i < numParticles; ++i) {
        m_particles.emplace_back();
    }
    for (const auto& p : m_particles) {
        m_cells[p.cellX][p.cellY].push_back(&p);
    }
}

AnimationState::~AnimationState() {
}

void AnimationState::drawParticles() const {
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

        // We check four neighbouring cells since some collisions may 
        // occur across cell boundaries
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

                const auto dx{ fabs(p.x - neighbour->x) };
                const auto dy{ fabs(p.y - neighbour->y) };
                constexpr auto radiusSq{ particleConnectionDistance * particleConnectionDistance };
                const auto distance{ dx * dx + dy * dy };

                // Draw a line to neighbouring particles. The line fades the further away it is.
                if (distance < radiusSq) {
                    const float distFactor{ 1.0f - distance / radiusSq };
                    glColor4f(1.0f, 1.0f, 1.0f, distFactor);
                    glBegin(GL_LINES); {
                        glVertex2f(p.x, p.y);
                        glVertex2f(neighbour->x, neighbour->y);
                    } glEnd();
                }
            }
        }
    }
}

constexpr bool resetGrid{ false };

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

        // printTimeToExecuteHighRes("Update", [this, dt]() {
            for (auto& p : m_particles) {
                p.update(*this, dt);
            }

            if constexpr (resetGrid) {
                updateSpacialPartitioningGrid();
            }
        // });
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
        p.cellX = static_cast<uint32_t>((p.x + 1.0f) / cellSize);
        p.cellY = static_cast<uint32_t>((p.y + 1.0f) / cellSize);

        m_cells[p.cellX][p.cellY].push_back(&p);
    }
}

Particle::Particle() :
    x{ particleMinPos + static_cast<float>(rand()) /
       (static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos))) },
    y{ particleMinPos + static_cast<float>(rand()) /
       (static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos))) },
    dirX{ particleMinPos + static_cast<float>(rand()) /
          (static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos))) },
    dirY{ particleMinPos + static_cast<float>(rand()) /
          (static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos))) },
    size{ particleMinSize + static_cast<float>(rand()) /
          (static_cast<float>(RAND_MAX/(particleMaxSize-particleMinSize))) },
    speed{ particleMinSpeed + static_cast<float>(rand()) /
           (static_cast<float>(RAND_MAX/(particleMaxSpeed-particleMinSpeed))) },
    cellX{ static_cast<uint32_t>((x + 1.0f) / cellSize) },
    cellY{ static_cast<uint32_t>((y + 1.0f) / cellSize) } {
}

void Particle::update(AnimationState& as, float dt) {
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

    if constexpr (!resetGrid) {
        const uint32_t newCellX{ static_cast<uint32_t>((x + 1.0f) / cellSize) };
        const uint32_t newCellY{ static_cast<uint32_t>((y + 1.0f) / cellSize) };

        // If we've shifted into a new cell, we have to update the cell's particle list
        if (newCellX != cellX || newCellY != cellY) {
            // remove from old cell
            auto& cell{ as.m_cells[cellX][cellY] };
            cell.erase(std::remove(cell.begin(), cell.end(), this), cell.end());

            // add to new cell
            as.m_cells[newCellX][newCellY].push_back(this);

            cellX = newCellX;
            cellY = newCellY;
        }
    }
}

void Particle::draw() const {
    constexpr auto circleLines = int32_t{ 10 };
    glPushMatrix(); {
        glTranslatef(x, y, 0.0f);

        glColor4f(PARTICLE_R, PARTICLE_G, PARTICLE_B, PARTICLE_A);
        // Draw a circle if the particle is large enough, otherwise draw tiny square
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