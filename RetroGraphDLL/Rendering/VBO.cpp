module Rendering.VBO;

import "GLHeaderUnit.h";

namespace rg {

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

} // namespace rg
