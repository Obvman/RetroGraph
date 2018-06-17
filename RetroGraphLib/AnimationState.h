#pragma once

#include "stdafx.h"

#include <cstdint>
#include <vector>
#include <array>

#include "Measure.h"

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

class Particle {
public:
    Particle();

    void update(AnimationState& as, float dt);

    float x{ 0.0f };
    float y{ 0.0f };
    float dirX{ 0.0f };
    float dirY{ 0.0f };
    float size{ 0.0f };
    float speed{ 0.0f };

    uint32_t cellX{ 0 };
    uint32_t cellY{ 0 };
private:
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

    /* Draws each particle */
    void drawParticles() const;

    uint32_t getAnimationFPS() const { return m_updateRates.front(); };
private:
    bool shouldUpdate(uint32_t ticks) const override;

    /* Draws the connecting line between particles that are close together */
    void drawParticleConnection(const Particle* p1,
                                const Particle* p2) const;

    std::vector<Particle> m_particles{ };

    // Members for spatial partitioning
    // The world space coordinates range from -1.0 to 1.0 for both x and y,
    // so we have a range of 2.0 for our world sides
    std::array<std::array<CellParticleList, numCellsPerSide>, numCellsPerSide> m_cells;

    GLuint m_circleList;

    friend class Particle;
};


} // namespace rg
