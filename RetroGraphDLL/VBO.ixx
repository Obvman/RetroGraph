export module Rendering.VBO;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export using VBOID = size_t;

constexpr GLuint invalidID{ UINT_MAX };

// Container for standard VBO data (no element array)
export class VBO {
public:
    VBO() noexcept : VBO{ 0 } { }
    explicit VBO(GLsizei size_) noexcept;
    ~VBO();

    VBO(VBO&& other);
    VBO& operator=(VBO&& other);

    GLuint id;
    GLsizei size; // Num vertices
    std::vector<GLfloat> data;
};

}
