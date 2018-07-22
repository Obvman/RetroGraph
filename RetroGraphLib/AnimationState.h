#pragma once

#include "stdafx.h"

#include <cstdint>
#include <vector>
#include <array>

#include "Measure.h"
#include "VBOController.h"

#ifndef __GL_H__
typedef unsigned int GLuint;
#endif

namespace rg {

constexpr size_t numParticles{ 100U };
constexpr size_t maxLines{ numParticles * numParticles };

// TODO alter this value based on size
constexpr auto particleConnectionDistance = float{ 0.2f };

// NOTE: numCellsPerSide == (2.0 / cellSize)
// cellSize should be no smaller than particleConnectionDistance
constexpr auto cellSize = float{ particleConnectionDistance };
constexpr auto numCellsPerSide = int{ 10 };

class AnimationState;

struct Particle {
    // Initialises members with random values. Should seed before constructing.
    Particle();
    ~Particle() = default;

    void update(AnimationState& as, float dt);

    float x{ 0.0f };
    float y{ 0.0f };
    float dirX{ 0.0f };
    float dirY{ 0.0f };
    float size{ 0.0f };
    float speed{ 0.0f };

    int cellX{ 0 };
    int cellY{ 0 };
};

struct ParticleLine {
    ParticleLine() = default;
    ParticleLine(float x1_, float y1_, float x2_, float y2_)
        : x1{ x1_ }
        , y1{ y1_ }
        , x2{ x2_ }
        , y2{ y2_ } { }
    ~ParticleLine() = default;

    float x1;
    float y1;
    float x2;
    float y2;
};

class AnimationState : public Measure {
    using CellParticleList = std::vector<const Particle*>;

public:
    AnimationState();
    ~AnimationState();

    /* Updates the positions of all particles */
    void update(int ticks) override;
    void refreshSettings() override;

    int getAnimationFPS() const { return m_updateRates.front(); };
    const std::array<ParticleLine, maxLines>& getLines() const { return m_particleLines; }
    const std::array<float, maxLines>& getLineColours() const { return m_lineColors; }
    const std::vector<Particle>& getParticles() const { return m_particles; }
    int getNumLines() const { return m_numLines; }

private:
    bool shouldUpdate(int ticks) const override;

    void updateParticleLines();
    void addLine(const Particle* const p1, const Particle* const p2);

    std::vector<Particle> m_particles{ };

    // Static buffer set to the maximum possible number of lines existing in worst case 
    // scenario (all particles are in neighbouring cells)
    std::array<ParticleLine, maxLines> m_particleLines;
    std::array<float, maxLines> m_lineColors;
    int m_numLines; // Tracks actual number of lines

    auto createParticles() -> decltype(m_particles);

    // Members for spatial partitioning
    // The world space coordinates range from -1.0 to 1.0 for both x and y,
    // so we have a range of 2.0 for our world sides
    std::array<std::array<CellParticleList, numCellsPerSide>, numCellsPerSide> m_cells;

    friend struct Particle;
};


} // namespace rg
