#pragma once

#include <GL/glew.h>
#include <string>

namespace rg {

/* Compiles and links the given vertex and fragment shaders */
GLuint loadShader(const std::string& vFile, const std::string& fFile);

}