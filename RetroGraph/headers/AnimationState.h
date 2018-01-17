#pragma once

#include <cstdint>
#include <vector>

namespace rg {

constexpr uint32_t animationFPS{ 45U };

constexpr float PARTICLE_R{ 0.8f };
constexpr float PARTICLE_G{ 0.8f };
constexpr float PARTICLE_B{ 0.8f };
constexpr float PARTICLE_A{ 0.5f };

constexpr float particleMinSize{ 0.005f };
constexpr float particleMaxSize{ 0.012f };

constexpr float particleMinPos{ -0.998f };
constexpr float particleMaxPos{ 0.998f };
constexpr float particleMinSpeed{ 0.01f };
constexpr float particleMaxSpeed{ 0.1f };

// TODO alter this value based on size
constexpr float particleConnectionDistance{ 0.2f };

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
private:
};

class AnimationState {
public:
    AnimationState();
    ~AnimationState();

    void update(uint32_t ticks);
    void drawParticles() const;

private:
    std::vector<Particle> m_particles{ };
};

}
