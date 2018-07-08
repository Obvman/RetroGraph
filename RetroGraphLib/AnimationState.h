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

constexpr float PARTICLE_R{ 0.8f };
constexpr float PARTICLE_G{ 0.8f };
constexpr float PARTICLE_B{ 0.8f };
constexpr float PARTICLE_A{ 0.5f };

// TODO alter this value based on size
constexpr float particleConnectionDistance{ 0.2f };

// NOTE: numCellsPerSide == (2.0 / cellSize)
// cellSize should be no smaller than particleConnectionDistance
constexpr float cellSize{ particleConnectionDistance };
const int32_t numCellsPerSide{ 10 };

class AnimationState;

struct Particle {
    Particle();
    ~Particle() = default;

    void update(AnimationState& as, float dt);

    float x{ 0.0f };
    float y{ 0.0f };
    float dirX{ 0.0f };
    float dirY{ 0.0f };
    float size{ 0.0f };
    float speed{ 0.0f };

    uint32_t cellX{ 0 };
    uint32_t cellY{ 0 };
};

struct ParticleLine {
    ParticleLine() = default;
    ParticleLine(float x1_, float y1_, float x2_, float y2_, float alpha_)
        : x1{ x1_ }
        , y1{ y1_ }
        , x2{ x2_ }
        , y2{ y2_ }
        , alpha{ alpha_ } { }
    ~ParticleLine() = default;

    float x1;
    float y1;
    float x2;
    float y2;
    float alpha;
};

class AnimationState : public Measure {
    using CellParticleList = std::vector<const Particle*>;

public:
    AnimationState();
    ~AnimationState();
    AnimationState(const AnimationState&) = delete;
    AnimationState& operator=(const AnimationState&) = delete;
    AnimationState(AnimationState&&) = delete;
    AnimationState& operator=(AnimationState&&) = delete;

    /* Updates the positions of all particles */
    void update(uint32_t ticks) override;

    uint32_t getAnimationFPS() const { return m_updateRates.front(); };

    void setCircleList(GLuint circleList) { m_circleList = circleList; }

    const ParticleLine& getLine(int i) const { return m_particleLines[i]; }
    const std::vector<Particle>& getParticles() const { return m_particles; }
    int getNumLines() const { return m_numLines; }
private:
    bool shouldUpdate(uint32_t ticks) const override;

    void updateParticleLines();
    void addLine(const Particle* const p1, const Particle* const p2);


    std::vector<Particle> m_particles{ };
    std::vector<ParticleLine> m_particleLines;
    int m_numLines;

    auto createParticles() -> decltype(m_particles);

    // Members for spatial partitioning
    // The world space coordinates range from -1.0 to 1.0 for both x and y,
    // so we have a range of 2.0 for our world sides
    std::array<std::array<CellParticleList, numCellsPerSide>, numCellsPerSide> m_cells;

    GLuint m_circleList;

    friend struct Particle;
};


} // namespace rg
