#pragma once

#include "stdafx.h"

#include <GL/glew.h>
#include <string>

namespace rg {

/* Compiles and links the given vertex and fragment shaders returns the ID
   of the compiled program if successful, otherwise presents a fatal
   message box with the GLSL compilation error message */
GLuint loadShader(const std::string& vFile, const std::string& fFile);

}