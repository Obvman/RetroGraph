export module Measures.AnimationState;

import UserSettings;

import Measures.Measure;
import Measures.Particle;
import Measures.ParticleLine;

import std.core;

namespace rg {

constexpr size_t numParticles{ 100U };
export constexpr size_t maxLines{ numParticles * numParticles };

export class AnimationState : public Measure {

public:
    AnimationState();
    ~AnimationState();

    /* Updates the positions of all particles */
    void update(int ticks) override;

    int getAnimationFPS() const { return m_animationFPS; };
    const std::array<ParticleLine, maxLines>& getLines() const { return m_particleLines; }
    const std::vector<Particle>& getParticles() const { return m_particles; }
    int getNumLines() const { return m_numLines; }

private:
    bool shouldUpdate(int ticks) const override;

    void updateParticleLines();
    void addLine(const Particle* const p1, const Particle* const p2);

    std::vector<Particle> m_particles{ };
    auto createParticles() -> decltype(m_particles);

    // Static buffer set to the maximum possible number of lines existing in worst case 
    // scenario (all particles are in neighbouring cells)
    std::array<ParticleLine, maxLines> m_particleLines;
    int m_numLines; // Tracks actual number of lines

    // Members for spatial partitioning
    // The world space coordinates range from -1.0 to 1.0 for both x and y,
    // so we have a range of 2.0 for our world sides
    Cells m_cells;

    int m_animationFPS;
    RefreshProcHandle m_refreshProcHandle;

    friend struct Particle;
};

} // namespace rg
