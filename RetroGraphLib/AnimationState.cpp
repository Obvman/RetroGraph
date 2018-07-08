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
#include "drawUtils.h"
#include "UserSettings.h"
#include "ListContainer.h"
#include "VBOController.h"

namespace rg {

constexpr size_t numParticles{ 100U };
constexpr auto numVerticesPerCircle{ circleLines + 2 };
constexpr auto numCoordsPerCircle{ numVerticesPerCircle * 2 };

constexpr float particleMinSize{ 0.005f };
constexpr float particleMaxSize{ 0.012f };
constexpr float particleMinPos{ -0.998f };
constexpr float particleMaxPos{ 0.998f };
constexpr float particleMinSpeed{ 0.01f };
constexpr float particleMaxSpeed{ 0.1f };

// TODO Debug values
//constexpr float particleMinSpeed{ 0.3f };
//constexpr float particleMaxSpeed{ 0.6f };
//constexpr float particleMinSize{ 0.012f };
//constexpr float particleMaxSize{ 0.036f };

AnimationState::AnimationState()
    : Measure{ std::get<uint32_t>(UserSettings::inst().getVal("Widgets-Main.FPS")) }
    , m_particles( createParticles() ) {
    //, m_vboID{ VBOController::inst().createVBO(m_particles.size()) } {

    for (const auto& p : m_particles) {
        m_cells[p.cellX][p.cellY].push_back(&p);
    }
}

AnimationState::~AnimationState() {
}

void AnimationState::drawParticles() const {
    // Draw the particle itself
    for (const auto& p : m_particles) {
        glPushMatrix(); {
            glTranslatef(p.x, p.y, 0.0f);
            glScalef(p.size, p.size, 1.0f);
            ListContainer::inst().drawCircle();
        } glPopMatrix();
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
        const std::array<const CellParticleList*, 4> neighbouringCells = {
            &(m_cells[p.cellX][nextY]),
            &(m_cells[nextX][p.cellY]),
            &(m_cells[nextX][prevY]),
            &(m_cells[nextX][nextY]),
        };

        for (const auto* cell : neighbouringCells) {
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

bool AnimationState::shouldUpdate(uint32_t ticks) const {
    return ticksMatchRate(ticks, m_updateRates.front());
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

auto AnimationState::createParticles() -> decltype(m_particles) {
    decltype(m_particles) particleList;

    std::srand(static_cast<uint32_t>(time(nullptr)));
    for (auto i = size_t{ 0U }; i < numParticles; ++i) {
        particleList.emplace_back();
    }
    return particleList;
}

void AnimationState::update(uint32_t) {
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

// TODO this is bugged, particles get stuck in the corners of the window
void Particle::update(AnimationState& as, float dt) {
    x += speed * dirX * dt;
    y += speed * dirY * dt;

    // If we move off the screen, wrap the particle around to the other side
    if (x < particleMinPos) {
        x = particleMaxPos;
        y = -y;
    } else if (x > particleMaxPos) {
        x = particleMinPos;
        y = -y;
    } 
    if (y < particleMinPos) {
        x = -x;
        y = particleMaxPos;
    } else if (y > particleMaxPos) {
        x = -x;
        y = particleMinPos;
    }

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
