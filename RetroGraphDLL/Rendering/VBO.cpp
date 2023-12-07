module Rendering.VBO;

import "GLHeaderUnit.h";
import "RGAssert.h";

namespace rg {

VBO::VBO(GLenum target_, GLenum usage_) noexcept
    : id{ invalidID }
    , target{ target_ }
    , usage{ usage_ } {

    glGenBuffers(1, &id);
}

VBO::~VBO() {
    if (id != invalidID) {
        glDeleteBuffers(1, &id);
    }
}

VBO::VBO(VBO&& other) {
    *this = std::move(other);
}

VBO& VBO::operator=(VBO&& other) {
    if (this != &other) {
        id = other.id;
        other.id = invalidID;

        target = other.target;
        usage = other.usage;
    }

    return *this;
}

}
