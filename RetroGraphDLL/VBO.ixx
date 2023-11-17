export module Rendering.VBO;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

constexpr GLuint invalidID{ UINT_MAX };

// Container for standard VBO data (no element array)
export struct VBO {
    VBO() noexcept : VBO{ 0 } { }
    explicit VBO(GLsizei size_) noexcept;
    ~VBO();

    GLuint id;
    GLsizei size; // Num vertices
    std::vector<GLfloat> data;
};

VBO::VBO(GLsizei size_) noexcept
    : id{ invalidID }
    , size{ size_ }
    , data( size_ * 2) {

    glGenBuffers(1, &id);
}

VBO::~VBO() {
    if (id != invalidID)
        glDeleteBuffers(1, &id);
}

}
