#pragma once

#include "stdafx.h"

#include <GL/glew.h>
#include <string>

namespace rg {

/* Compiles and links the given vertex and fragment shaders returns the ID
   of the compiled program if successful, otherwise presents a fatal
   message box with the GLSL compilation error message */
class GLShader {
public:
    GLShader(const std::string& vertPath, const std::string& fragPath)
        : id{ loadShader(vertPath, fragPath) } { }
    GLShader(const std::string& baseName)
        : GLShader{ baseName + ".vert", baseName + ".frag" } { }
    ~GLShader() { if (*this) glDeleteProgram(id); }

    operator GLuint() { return id; }
    operator bool() { return id > 0; }

private:
    GLuint loadShader(const std::string& vFile, const std::string& fFile);

    GLuint id;
};

}