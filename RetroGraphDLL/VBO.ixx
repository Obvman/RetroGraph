export module Rendering.VBO;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export using VBOID = size_t;
export using VAOID = size_t;

constexpr GLuint invalidID{ UINT_MAX };

// Container for standard VBO data (no element array)
export template<class T>
class VBO {
public:
    VBO() noexcept : VBO{ 0 } { }

    explicit VBO(GLsizei size_) noexcept
        : id{ invalidID }
        , size{ size_ }
        , data( size_ * 2) {

        glGenBuffers(1, &id);
    }

    ~VBO() {
        if (id != invalidID)
            glDeleteBuffers(1, &id);
    }

    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;

    VBO(VBO&& other) {
        *this = std::move(other);
    }

    VBO& operator=(VBO&& other) {
        if (this != &other) {
            id = other.id;
            other.id = invalidID;

            size = other.size;
            other.size = 0;

            data = std::move(other.data);
        }

        return *this;
    }

    GLuint id;
    GLsizei size; // Num vertices
    std::vector<T> data;
};


}
