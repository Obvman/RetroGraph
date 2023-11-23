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

VBO::VBO(VBO&& other) {
    *this = std::move(other);
}

VBO& VBO::operator=(VBO&& other) {
    if (this != &other) {
        id = other.id;
        other.id = invalidID;

        size = other.size;
        other.size = 0;

        data = std::move(other.data);
    }

    return *this;
}

} // namespace rg
