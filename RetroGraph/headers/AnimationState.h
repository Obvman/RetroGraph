#pragma once

#include <cstdint>
#include <vector>
#include <array>

namespace rg {

constexpr float PARTICLE_R{ 0.8f };
constexpr float PARTICLE_G{ 0.8f };
constexpr float PARTICLE_B{ 0.8f };
constexpr float PARTICLE_A{ 0.5f };

constexpr size_t numParticles{ 100U };
constexpr float particleMinSize{ 0.005f };
constexpr float particleMaxSize{ 0.012f };

constexpr float particleMinPos{ -0.998f };
constexpr float particleMaxPos{ 0.998f };
constexpr float particleMinSpeed{ 0.01f };
constexpr float particleMaxSpeed{ 0.1f };

// TODO alter this value based on size
constexpr float particleConnectionDistance{ 0.2f };

// NOTE: numCellsPerSide == (2.0 / cellSize + 2)
constexpr float cellSize{ 0.25f };
const int32_t numCellsPerSide{ 8 };

class Particle {
public:
    Particle();
    Particle(float x_, float y_, float dirX_, float dirY_, float size_);

    void draw() const;
    void update(float dt);

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
    void updateSpacialPartitioningGrid();

    std::vector<Particle> m_particles{ };
    uint32_t m_animationFPS{ 20U };

    // Members for spatial partitioning
    // The world space coordinates range from -1.0 to 1.0 for both x and y,
    // so we have a range of 2.0 for our world sides
    std::array<std::array<std::vector<const Particle*>, numCellsPerSide>, numCellsPerSide> m_cells;
};

}
