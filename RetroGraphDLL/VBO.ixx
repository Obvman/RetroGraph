export module Rendering.VBO;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export using VBOID = size_t;

constexpr GLuint invalidID{ UINT_MAX };

export class [[nodiscard]] VBOBindScope {
public:
    VBOBindScope(GLuint id, GLenum target_)
        : target{ target_ } {

        glBindBuffer(target, id);
    }
    ~VBOBindScope() {
        glBindBuffer(target, 0);
    }
    VBOBindScope(const VBOBindScope&) = delete;
    VBOBindScope& operator=(const VBOBindScope&) = delete;
    VBOBindScope(VBOBindScope&&) = delete;
    VBOBindScope& operator=(VBOBindScope&&) = delete;

private:
    GLenum target;
};

// Container for standard VBO data (no element array)
export template<class T>
class VBO {
public:
    VBO(GLenum target) noexcept : VBO{ 0, target } { }

    explicit VBO(GLsizei size_, GLenum target_) noexcept
        : id{ invalidID }
        , data( size_ )
        , target{ target_ } {

        glGenBuffers(1, &id);
    }

    ~VBO() {
        if (id != invalidID) {
            glDeleteBuffers(1, &id);
        }
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

            target = other.target;

            data = std::move(other.data);
        }

        return *this;
    }

    VBOBindScope bind() const { return { id, target }; }
    GLsizei size() const { return static_cast<GLsizei>(data.size()); }
    GLsizei sizeBytes() const { return size() * sizeof(T); }
    GLsizei elemBytes() const { return sizeof(T); }

    void resize(size_t size) { data.resize(size); }
    void reserve(size_t size) { data.reserve(size); }

    std::vector<T> data;

private:
    GLuint id;
    GLenum target;
};

}
