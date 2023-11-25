module Measures.Particle;

import "CSTDHeaderUnit.h";

namespace rg {

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
void Particle::update(Cells& cells, float dt) {
    x += speed * dirX * dt;
    y += speed * dirY * dt;

    // If we move off the screen, wrap the particle around to the other side
    bool teleported{ false };
    if (x < particleMinPos) {
        x = particleMaxPos;
        y = -y;
        teleported = true;
    } else if (x > particleMaxPos) {
        x = particleMinPos;
        y = -y;
        teleported = true;
    } 

    // Check the need to wrap in the y direction ONLY if we didn't wrap in the x direction,
    // otherwise we'll just put the particle back in the same spot and it will be stuck forever
    if (!teleported) {
        if (y < particleMinPos) {
            x = -x;
            y = particleMaxPos;
        } else if (y > particleMaxPos) {
            x = -x;
            y = particleMinPos;
        }
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

} // namespace rg
