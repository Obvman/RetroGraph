#include "../headers/AnimationState.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>

#include "../headers/units.h"

namespace rg {

AnimationState::AnimationState() {
    m_particles = std::vector<Particle>{ };
    // m_particles.emplace_back(-0.9f, 0.9f, 0.0f, 0.3f, 0.005f);
    // m_particles.emplace_back( 0.2f, -0.3f, 0.1f, -0.2f, 0.005f );

    srand(static_cast<uint32_t>(time(nullptr)));
    for (auto i = size_t{ 0U }; i < 100; ++i) {
        m_particles.emplace_back();
    }
}


AnimationState::~AnimationState() {
}

void AnimationState::drawParticles() const {
    for (const auto& p : m_particles) {
        p.draw();


        // std::vector<const Particle*> collisions{};
        for (const auto& neighbour : m_particles) {
            if (&p == &neighbour) continue;

            const auto dx{ fabs(p.x - neighbour.x) };
            const auto dy{ fabs(p.y - neighbour.y) };
            constexpr auto radiusSq{ particleConnectionDistance * particleConnectionDistance };
            const auto distance{ dx * dx + dy * dy };

            // if (dx < radius && dy < radius) {
            if (distance < radiusSq) {
                // collisions.push_back(&neighbour);
                const float distFactor{ 1.0f - distance / radiusSq };
                glColor4f(1.0f, 1.0f, 1.0f, distFactor);
                glBegin(GL_LINES); {
                    glVertex2f(p.x, p.y);
                    glVertex2f(neighbour.x, neighbour.y);
                } glEnd();
            }
        }
    }
}

void AnimationState::update(uint32_t ticks) {
    if (ticks % (ticksPerSecond / animationFPS) == 0) {
        using clock = std::chrono::high_resolution_clock;
        static auto time_start = clock::now();

        auto time_end = clock::now();
        const auto deltaTimeStep{ time_end - time_start };
        time_start = clock::now();
        const auto dt{ std::chrono::duration_cast<std::chrono::duration<float>>(deltaTimeStep).count() };

        for (auto& p : m_particles) {
            p.update(dt);
        }
    }
}

Particle::Particle() {
    constexpr auto seed = 10U;

    // Randomly initialise each particle
    x = particleMinPos + static_cast<float> (rand()) /(static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos)));
    y = particleMinPos + static_cast<float> (rand()) /(static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos)));
    dirX = particleMinPos + static_cast<float> (rand()) /(static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos)));
    dirY = particleMinPos + static_cast<float> (rand()) /(static_cast<float>(RAND_MAX/(particleMaxPos-particleMinPos)));
    size = particleMinSize + static_cast<float> (rand()) /(static_cast<float>(RAND_MAX/(particleMaxSize-particleMinSize)));
    speed = particleMinSpeed + static_cast<float> (rand()) /(static_cast<float>(RAND_MAX/(particleMaxSpeed-particleMinSpeed)));
}

Particle::Particle(float x_, float y_, float dirX_, float dirY_, float size_) :
    x{ x_ }, y{ y_ },
    dirX{ dirX_ }, dirY{ dirY_ },
    size{ size_ }, speed{ 0.01f } {


}

void Particle::update(float dt) {
    x += speed * dirX * dt;
    y += speed * dirY * dt;

    // If we move off the screen, wrap the particle around to the other side
    if (x <= -1.0f) {
        x = 1.0f;
        y = -y;
    } else if (y <= -1.0f) {
        x = -x;
        y = 1.0f;
    } else if (x >= 1.0f) {
        x = -1.0f;
        y = -y;
    } else if (y >= 1.0f) {
        x = -x;
        y = -1.0f;
    }
}

void Particle::draw() const {
    constexpr auto circleLines = int32_t{ 10 };
    glPushMatrix(); {
        glTranslatef(x, y, 0.0f);

        glColor4f(PARTICLE_R, PARTICLE_G, PARTICLE_B, PARTICLE_A);
        if (size > particleMinSize * 2) {
            glBegin(GL_TRIANGLE_FAN); {
                glVertex2f(0.0f, 0.0f);
                for (int i = 0; i < circleLines; ++i) {
                    const auto theta{ 2.0f * 3.1415926f * static_cast<float>(i) /
                        static_cast<float>(circleLines - 1) };
                    glVertex2f( size * cosf(theta) ,  size * sinf(theta) );
                }
            } glEnd();
        } else {
            glBegin(GL_QUADS); {
                glVertex2f(-size/2.0f, size/2.0f);
                glVertex2f(size/2.0f, size/2.0f);
                glVertex2f(size/2.0f, -size/2.0f);
                glVertex2f(-size/2.0f, -size/2.0f);
            } glEnd();
        }

    } glPopMatrix();
}

}