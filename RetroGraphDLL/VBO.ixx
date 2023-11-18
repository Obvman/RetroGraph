export module Rendering.VBO;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export using VBOID = size_t;

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

}
