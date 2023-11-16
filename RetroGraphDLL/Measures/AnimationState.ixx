module;

export module Measures.AnimationState;

import Colors;
import FPSLimiter; // Invalid dependency
import Units;
import UserSettings;
import Utils;
import Measures.Measure;
import Measures.Particle;
import Rendering.DrawUtils; // Invalid dependency
import Rendering.GLListContainer; // Invalid dependency

import std.core;

import "GLHeaders.h";
import "WindowsHeaders.h";

namespace rg {

constexpr size_t numParticles{ 100U };
export constexpr size_t maxLines{ numParticles * numParticles };

constexpr auto numVerticesPerCircle{ circleLines + 2 };
constexpr auto numCoordsPerCircle{ numVerticesPerCircle * 2 };

export class AnimationState : public Measure {

public:
    AnimationState();
    ~AnimationState();

    /* Updates the positions of all particles */
    void update(int ticks) override;
    void refreshSettings() override;

    int getAnimationFPS() const { return m_animationFPS; };
    const std::array<ParticleLine, maxLines>& getLines() const { return m_particleLines; }
    const std::array<float, maxLines>& getLineColours() const { return m_lineColors; }
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
    std::array<float, maxLines> m_lineColors;
    int m_numLines; // Tracks actual number of lines

    // Members for spatial partitioning
    // The world space coordinates range from -1.0 to 1.0 for both x and y,
    // so we have a range of 2.0 for our world sides
    CellContainer m_cells;

    int m_animationFPS;

    friend struct Particle;
};

AnimationState::AnimationState()
    : m_particles( createParticles() )
    , m_particleLines{}
    , m_numLines{ 0 }
    , m_animationFPS{ UserSettings::inst().getVal<int>("Widgets-Main.FPS") } {

    for (const auto& p : m_particles)
        m_cells[p.cellX][p.cellY].push_back(&p);
}

AnimationState::~AnimationState() {
}

auto AnimationState::createParticles() -> decltype(m_particles) {
    std::srand(static_cast<unsigned int>(_time64(nullptr)));
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
        p.update(m_cells, dt);

    updateParticleLines();
}

void AnimationState::refreshSettings() {
    m_animationFPS = UserSettings::inst().getVal<int>("Widgets-Main.FPS");
    FPSLimiter::inst().setMaxFPS(m_animationFPS);
}

bool AnimationState::shouldUpdate(int ticks) const {
    return ticksMatchRate(ticks, m_animationFPS);
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

} // namespace rg
