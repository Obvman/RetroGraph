#pragma once

#include <cstdint>
#include <vector>
#include <array>

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

    void draw() const;
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

class AnimationState {
public:
    AnimationState();
    ~AnimationState();

    void update(uint32_t ticks);
    void drawParticles() const;

    uint32_t getAnimationFPS() const { return m_animationFPS; };
private:
    void drawParticleConnection(const Particle* const p1,
                                const Particle* const p2) const;
    void drawCells() const;

    std::vector<Particle> m_particles{ };
    uint32_t m_animationFPS{ 20U };

    // Members for spatial partitioning
    // The world space coordinates range from -1.0 to 1.0 for both x and y,
    // so we have a range of 2.0 for our world sides
    std::array<
        std::array<std::vector<const Particle*>, numCellsPerSide>, 
        numCellsPerSide> m_cells;

    friend class Particle;
};


}
