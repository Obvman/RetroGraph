export module RG.Measures:AnimationState;

import :Measure;
import :Particle;
import :ParticleLine;

import std.core;

namespace rg {

constexpr size_t numParticles{ 100U };
export constexpr size_t maxLines{ numParticles * numParticles };

export class AnimationState : public Measure {

public:
    AnimationState();
    ~AnimationState() = default;

    const std::array<ParticleLine, maxLines>& getLines() const { return m_particleLines; }
    const std::vector<Particle>& getParticles() const { return m_particles; }
    int getNumLines() const { return m_numLines; }

protected:

    /* Updates the positions of all particles */
    bool updateInternal() override;

private:
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

    friend struct Particle;
};

} // namespace rg
