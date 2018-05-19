#include "stdafx.h"

#include "AnimationState.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <iostream>

// #include <GL/glew.h>
// #include <GL/gl.h>

#include "units.h"
#include "utils.h"
#include "UserSettings.h"

namespace rg {

constexpr size_t numParticles{ 100U };
constexpr auto circleLines = int32_t{ 10 };
constexpr auto numVerticesPerCircle{ circleLines + 2 };
constexpr auto numCoordsPerCircle{ numVerticesPerCircle * 2 };

constexpr float particleMinSize{ 0.005f };
constexpr float particleMaxSize{ 0.012f };
constexpr float particleMinPos{ -0.998f };
constexpr float particleMaxPos{ 0.998f };
constexpr float particleMinSpeed{ 0.01f };
constexpr float particleMaxSpeed{ 0.1f };

AnimationState::AnimationState() : 
        m_particles{ },
        m_animationFPS{ std::get<uint32_t>(
                            UserSettings::inst().getVal("Widgets-Main.FPS")
                        ) },
        m_circleList{ glGenLists(1) } {

    // Generate particles and populate the cell particle observer lists.
    srand(static_cast<uint32_t>(time(nullptr)));
    for (auto i = size_t{ 0U }; i < numParticles; ++i) {
        m_particles.emplace_back();
    }
    for (const auto& p : m_particles) {
        m_cells[p.cellX][p.cellY].push_back(&p);
    }

    // Create a static circle used to draw each particle.
    glNewList(m_circleList, GL_COMPILE);
    glBegin(GL_TRIANGLE_FAN); {
        glVertex2f(0.0f, 0.0f);
        for (int i = 0; i < circleLines; ++i) {
            const auto theta{ 2.0f * 3.1415926f * static_cast<float>(i) /
                static_cast<float>(circleLines - 1) };
            glVertex2f(cosf(theta), sinf(theta));
        }
    } glEnd();
    glEndList();


}

AnimationState::~AnimationState() {
    glDeleteLists(m_circleList, 1);
}

void AnimationState::drawParticles() const {
    // Draw the particle itself
    for (const auto& p : m_particles) {
        glPushMatrix();
        glTranslatef(p.x, p.y, 0.0f);
        glScalef(p.size, p.size, 1.0f);
        glCallList(m_circleList);
        glPopMatrix();
    }

    //  draw lines to particles in neighbouring cells
    // (but not the current cell)
    for (const auto& p : m_particles) {
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
        const auto neighbouringCells = {
            &(m_cells[p.cellX][nextY]),
            &(m_cells[nextX][p.cellY]),
            &(m_cells[nextX][prevY]),
            &(m_cells[nextX][nextY]),
        };

        for (const auto cell : neighbouringCells) {
            for (const auto neighbour : *cell) {
                drawParticleConnection(&p, neighbour);
            }
        }
    }

    // Handle the current cell particle lines here such that we only
    // test each pair once
    for (const auto& row : m_cells) {
        for (const auto& cell : row) {
            const auto cellParticles{ cell.size() };
            for (auto i = size_t{ 0U }; i < cellParticles; ++i) {
                // j starts at i+1 so we don't duplicate collision checks.
                for (auto j = size_t{ i + 1 }; j < cellParticles; ++j) {
                    drawParticleConnection(cell[i], cell[j]);
                }
            }
        }
    }
}

void AnimationState::drawParticleConnection(const Particle* const p1,
                                            const Particle* const p2) const {
    if (p1 == p2) return;

    const auto dx{ fabs(p1->x - p2->x) };
    const auto dy{ fabs(p1->y - p2->y) };
    constexpr auto radiusSq{ particleConnectionDistance * 
                             particleConnectionDistance };
    const auto distance{ dx * dx + dy * dy };

    // Draw a line to neighbouring particles. line fades the further away it is.
    if (distance < radiusSq) {
        const float distFactor{ 1.0f - distance / radiusSq };
        glColor4f(1.0f, 1.0f, 1.0f, distFactor);
        glBegin(GL_LINES); {
            glVertex2f(p1->x, p1->y);
            glVertex2f(p2->x, p2->y);
        } glEnd();
    }
}

void AnimationState::update(uint32_t ticks) {
    if (ticks % (ticksPerSecond / m_animationFPS) == 0) {
        using namespace std::chrono;
        using clock = std::chrono::high_resolution_clock;
        static auto time_start = clock::now();

        auto time_end = clock::now();
        const auto deltaTimeStep{ time_end - time_start };
        time_start = clock::now();
        auto dt{ duration_cast<duration<float>>(deltaTimeStep).count() };

        // If a lot of time has passed, set dt to a small value so we don't have
        // one large jump
        if (dt > 1.0f) {
            dt = 0.016f;
        }

        for (auto& p : m_particles) {
            p.update(*this, dt);
        }
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

    uint8_t timesTeleported{ 0U };

    // If we move off the screen, wrap the particle around to the other side
    if (x < particleMinPos) {
        x = particleMaxPos;
        y = -y;
        ++timesTeleported;
    } else if (x > particleMaxPos) {
        x = particleMinPos;
        y = -y;
        ++timesTeleported;
    } 
    if (y < particleMinPos) {
        x = -x;
        y = particleMaxPos;
        ++timesTeleported;
    } else if (y > particleMaxPos) {
        x = -x;
        y = particleMinPos;
        ++timesTeleported;
    }

    // This occurs when the particle has moved from one corner to
    // the opposite, and then it's moved back to the original corner,
    // so the particle is forever stuck. We just shift it a little to
    // keep it moving.
    /*if (timesTeleported == 2) {
        if (x > 0.0f) {
            x -= 0.1f;
        } else {
            x += 0.1f;
        }
    }*/

    const uint32_t newCellX{ static_cast<uint32_t>((x + 1.0f) / cellSize) };
    const uint32_t newCellY{ static_cast<uint32_t>((y + 1.0f) / cellSize) };

    // If we've shifted into a new cell, we have to update the cell's particle list
    if (newCellX != cellX || newCellY != cellY) {
        // remove from old cell
        auto& cell{ as.m_cells[cellX][cellY] };
        cell.erase(std::remove(cell.begin(), cell.end(), this), cell.end());

        // add to new cell
        as.m_cells[newCellX][newCellY].push_back(this); // still fukn bugged

        cellX = newCellX;
        cellY = newCellY;
    }
}

}
