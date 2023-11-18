export module Rendering.GLShader;

import Utils;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

// The executable path is different whether we start the program from the
// visual studio or debugger, so this handles the two cases
const std::string shaderPath{ getExePath() + R"(\..\..\RetroGraph\Resources\shaders\)" };

/* Compiles and links the given vertex and fragment shaders returns the ID
   of the compiled program if successful, otherwise presents a fatal
   message box with the GLSL compilation error message */
export class GLShader {
public:
    GLShader(const std::string& vertPath, const std::string& fragPath)
        : id{ loadShader(vertPath, fragPath) } { }
    GLShader(const std::string& baseName)
        : GLShader{ baseName + ".vert", baseName + ".frag" } { }
    ~GLShader();

    operator GLuint() { return id; }
    operator bool() { return id > 0; }

private:
    GLuint loadShader(const std::string& vFile, const std::string& fFile);
    std::string readShaderFile(const std::string& filePath) const;

    GLuint id;
};

} // namespace rg
