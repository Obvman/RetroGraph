export module Particle;

import <array>;
import <vector>;

namespace rg {

export {
    struct Particle;
    struct ParticleLine;

    // #TODO move to own module
    // NOTE: numCellsPerSide == (2.0 / cellSize)
    // cellSize should be no smaller than particleConnectionDistance
    constexpr auto particleConnectionDistance = float{ 0.2f };
    constexpr auto cellSize = float{ particleConnectionDistance };
    constexpr auto numCellsPerSide = int{ 10 };

    constexpr auto particleMinSize = float{ 0.005f };
    constexpr auto particleMaxSize = float{ 0.012f };
    constexpr auto particleMinPos = float{ -0.998f };
    constexpr auto particleMaxPos = float{ 0.998f };
    constexpr auto particleMinSpeed = float{ 0.01f };
    constexpr auto particleMaxSpeed = float{ 0.1f };

    using CellParticleList = std::vector<const Particle*>;
    using CellContainer = std::array<std::array<CellParticleList, numCellsPerSide>, numCellsPerSide>;
}


struct Particle {
    // Initialises members with random values. Should seed before constructing.
    Particle();
    ~Particle() = default;

    void update(CellContainer& as, float dt);

    float x{ 0.0f };
    float y{ 0.0f };
    float dirX{ 0.0f };
    float dirY{ 0.0f };
    float size{ 0.0f };
    float speed{ 0.0f };

    int cellX{ 0 };
    int cellY{ 0 };
};

struct ParticleLine {
    ParticleLine() = default;
    ParticleLine(float x1_, float y1_, float x2_, float y2_)
        : x1{ x1_ }
        , y1{ y1_ }
        , x2{ x2_ }
        , y2{ y2_ } { }
    ~ParticleLine() = default;

    float x1;
    float y1;
    float x2;
    float y2;
};

Particle::Particle()
    : x{ particleMinPos + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos))) }
    , y{ particleMinPos + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos))) }
    , dirX{ particleMinPos + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos))) }
    , dirY{ particleMinPos + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos))) }
    , size{ particleMinSize + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(particleMaxSize-particleMinSize))) }
    , speed{ particleMinSpeed + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(particleMaxSpeed-particleMinSpeed))) }
    , cellX{ static_cast<int>((x + 1.0f) / cellSize) }
    , cellY{ static_cast<int>((y + 1.0f) / cellSize) } {
}

// TODO this is bugged, particles get stuck in the corners of the window
void Particle::update(CellContainer& cells, float dt) {
    x += speed * dirX * dt;
    y += speed * dirY * dt;

    // If we move off the screen, wrap the particle around to the other side
    if (x < particleMinPos) {
        x = particleMaxPos;
        y = -y;
    } else if (x > particleMaxPos) {
        x = particleMinPos;
        y = -y;
    } 
    if (y < particleMinPos) {
        x = -x;
        y = particleMaxPos;
    } else if (y > particleMaxPos) {
        x = -x;
        y = particleMinPos;
    }

    const int newCellX{ static_cast<int>((x + 1.0f) / cellSize) };
    const int newCellY{ static_cast<int>((y + 1.0f) / cellSize) };

    // If we've shifted into a new cell, we have to update the cell's particle list
    if (newCellX != cellX || newCellY != cellY) {
        // remove from old cell
        auto& cell{ cells[cellX][cellY] };
        cell.erase(std::remove(cell.begin(), cell.end(), this), cell.end());

        // add to new cell
        cells[newCellX][newCellY].push_back(this);

        cellX = newCellX;
        cellY = newCellY;
    }
}


}
