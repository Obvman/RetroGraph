export module Rendering.Viewport;

import "GLHeaderUnit.h";

namespace rg {

export struct Viewport {
    GLint x{ 0 };
    GLint y{ 0 };
    GLint width{ 0 };
    GLint height{ 0 };

    Viewport() = default;
    Viewport(GLint _x, GLint _y, GLint _width, GLint _height) :
        x{ _x }, y{ _y }, width{ _width }, height{ _height } {}
    void set(GLint _x, GLint _y, GLint _width, GLint _height) {
        x = _x; y = _y; width = _width; height = _height;
    }
};

} // namespace rg
