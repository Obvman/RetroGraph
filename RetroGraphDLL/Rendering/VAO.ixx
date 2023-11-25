export module Rendering.VAO;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

constexpr GLuint invalidID{ UINT_MAX };

export class [[nodiscard]] VAOBindScope {
public:
    VAOBindScope(GLuint id) {
        glBindVertexArray(id);
    }
    ~VAOBindScope() {
        glBindVertexArray(0);
    }
    VAOBindScope(const VAOBindScope&) = delete;
    VAOBindScope& operator=(const VAOBindScope&) = delete;
    VAOBindScope(VAOBindScope&&) = delete;
    VAOBindScope& operator=(VAOBindScope&&) = delete;
};

export class VAO {
public:
    VAO() noexcept {
        glGenVertexArrays(1, &id);
    }

    ~VAO() {
        if (id != invalidID) {
            glDeleteVertexArrays(1, &id);
        }
    }

    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;

    VAO(VAO&& other) {
        *this = std::move(other);
    }

    VAO& operator=(VAO&& other) {
        if (this != &other) {
            id = other.id;
            other.id = invalidID;
        }

        return *this;
    }

    VAOBindScope bind() const { return { id }; }

private:
    GLuint id;
};

}
