module RG.Measures:AnimationState;

import RG.Core;
import RG.Rendering;

namespace rg {

AnimationState::AnimationState()
    : Measure{ milliseconds{ 0 } }
    , m_particles(createParticles())
    , m_particleLines{}
    , m_numLines{ 0 } {
    for (const auto& p : m_particles)
        m_cells[p.cellX][p.cellY].push_back(&p);
}

auto AnimationState::createParticles() -> decltype(m_particles) {
    std::srand(static_cast<unsigned int>(_time64(nullptr)));
    return decltype(m_particles)(numParticles);
}

bool AnimationState::updateInternal() {
    using namespace std::chrono;
    using clock = high_resolution_clock;

    const auto deltaTimeStep{ since<clock, clock::duration, microseconds>(m_lastUpdateTime) };
    auto dt{ duration_cast<duration<float>>(deltaTimeStep).count() };

    // If a lot of time has passed, set dt to a small value so we don't have one large jump
    if (dt > 1.0f)
        dt = 0.016f;

    for (auto& p : m_particles)
        p.update(m_cells, dt);

    updateParticleLines();

    return true;
}

void AnimationState::updateParticleLines() {
    m_numLines = 0;

    // draw lines to particles in neighbouring cells (but not the current cell)
    for (const auto& p : m_particles) {
        auto nextX = int{ p.cellX + 1 };
        auto nextY = int{ p.cellY + 1 };
        auto prevY = int{ static_cast<int>(p.cellY) - 1 }; // can be negative
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
    if (p1 == p2)
        return;

    constexpr auto radiusSq{ particleConnectionDistance * particleConnectionDistance };
    const auto dx{ fabs(p1->x - p2->x) };
    const auto dy{ fabs(p1->y - p2->y) };
    const auto distance{ dx * dx + dy * dy };

    if (distance < radiusSq) {
        m_particleLines[m_numLines++] = ParticleLine{ p1->x, p1->y, p2->x, p2->y };
    }
}

} // namespace rg
