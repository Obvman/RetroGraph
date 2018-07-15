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
#include "colors.h"
#include "drawUtils.h"
#include "UserSettings.h"
#include "ListContainer.h"
#include "VBOController.h"
#include "FPSLimiter.h"

namespace rg {

constexpr auto numVerticesPerCircle{ circleLines + 2 };
constexpr auto numCoordsPerCircle{ numVerticesPerCircle * 2 };

constexpr float particleMinSize{ 0.005f };
constexpr float particleMaxSize{ 0.012f };
constexpr float particleMinPos{ -0.998f };
constexpr float particleMaxPos{ 0.998f };
constexpr float particleMinSpeed{ 0.01f };
constexpr float particleMaxSpeed{ 0.1f };

AnimationState::AnimationState()
    : Measure{ UserSettings::inst().getVal<int>("Widgets-Main.FPS") }
    , m_particles( createParticles() )
    , m_particleLines{}
    , m_numLines{ 0 } {

    for (const auto& p : m_particles)
        m_cells[p.cellX][p.cellY].push_back(&p);
}

AnimationState::~AnimationState() {
}

auto AnimationState::createParticles() -> decltype(m_particles) {
    std::srand(static_cast<unsigned int>(time(nullptr)));
    return decltype(m_particles)( numParticles );
}

void AnimationState::update(int) {
    using namespace std::chrono;
    using clock = std::chrono::high_resolution_clock;

    static auto time_start = clock::now();

    const auto time_end = clock::now();
    const auto deltaTimeStep{ time_end - time_start };
    time_start = clock::now();
    auto dt{ duration_cast<duration<float>>(deltaTimeStep).count() };

    // If a lot of time has passed, set dt to a small value so we don't have one large jump
    if (dt > 1.0f)
        dt = 0.016f;

    for (auto& p : m_particles)
        p.update(*this, dt);

    updateParticleLines();
}

void AnimationState::refreshSettings() {
    m_updateRates.front() = UserSettings::inst().getVal<int>("Widgets-Main.FPS");
    FPSLimiter::inst().setMaxFPS(m_updateRates.front());
}

bool AnimationState::shouldUpdate(int ticks) const {
    return ticksMatchRate(ticks, m_updateRates.front());
}

void AnimationState::updateParticleLines() {
    m_numLines = 0;

    // draw lines to particles in neighbouring cells (but not the current cell)
    for (const auto& p : m_particles) {
        auto nextX = int{ p.cellX + 1 };
        auto nextY = int{ p.cellY + 1};
        auto prevY = int{ static_cast<int>(p.cellY) - 1}; // can be negative
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
            for (const auto neighbour : *cell)
                addLine(&p, neighbour);
        }
    }

    // Handle the current cell particle lines here such that we only
    // test each pair once
    for (const auto& row : m_cells) {
        for (const auto& cell : row) {
            const auto cellParticles{ cell.size() };
            for (auto i = size_t{ 0U }; i < cellParticles; ++i) {
                // j starts at i+1 so we don't duplicate collision checks.
                for (auto j = size_t{ i + 1 }; j < cellParticles; ++j)
                    addLine(cell[i], cell[j]);
            }
        }
    }

}

void AnimationState::addLine(const Particle* const p1, const Particle* const p2) {
    if (p1 == p2) return;

    constexpr auto radiusSq{ particleConnectionDistance * particleConnectionDistance };
    const auto dx{ fabs(p1->x - p2->x) };
    const auto dy{ fabs(p1->y - p2->y) };
    const auto distance{ dx * dx + dy * dy };

    if (distance < radiusSq) {
        m_particleLines[m_numLines] = ParticleLine{ p1->x, p1->y, p2->x, p2->y };
        m_lineColors[m_numLines++] = 1.0f - distance / radiusSq;
    }
}


Particle::Particle() :
    x{ particleMinPos + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos))) },
    y{ particleMinPos + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos))) },
    dirX{ particleMinPos + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos))) },
    dirY{ particleMinPos + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos))) },
    size{ particleMinSize + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(particleMaxSize-particleMinSize))) },
    speed{ particleMinSpeed + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(particleMaxSpeed-particleMinSpeed))) },
    cellX{ static_cast<int>((x + 1.0f) / cellSize) },
    cellY{ static_cast<int>((y + 1.0f) / cellSize) } {
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

    const int newCellX{ static_cast<int>((x + 1.0f) / cellSize) };
    const int newCellY{ static_cast<int>((y + 1.0f) / cellSize) };

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
