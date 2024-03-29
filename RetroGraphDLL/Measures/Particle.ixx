export module RG.Measures:Particle;

import std.core;

namespace rg {

struct Particle;

export {
    // NOTE: numCellsPerSide == (2.0 / cellSize)
    // cellSize should be no smaller than particleConnectionDistance
    constexpr auto particleConnectionDistance = float{ 0.2f };
    constexpr auto cellSize = float{ particleConnectionDistance };
    constexpr auto numCellsPerSide = int{ 10 };

    constexpr auto particleMinSize = float{ 1.0f };
    constexpr auto particleMaxSize = float{ 10.0f };
    constexpr auto particleMinPos = float{ -0.998f };
    constexpr auto particleMaxPos = float{ 0.998f };
    constexpr auto particleMinSpeed = float{ 0.01f };
    constexpr auto particleMaxSpeed = float{ 0.1f };

    using CellParticleList = std::vector<const Particle*>;
    using Cells = std::array<std::array<CellParticleList, numCellsPerSide>, numCellsPerSide>;
}

export struct Particle {
    // Initialises members with random values. Should seed before constructing.
    Particle();

    void update(Cells& cells, float dt);

    float x{ 0.0f };
    float y{ 0.0f };
    float dirX{ 0.0f };
    float dirY{ 0.0f };
    float size{ 0.0f };
    float speed{ 0.0f };

    int cellX{ 0 };
    int cellY{ 0 };
};

} // namespace rg
