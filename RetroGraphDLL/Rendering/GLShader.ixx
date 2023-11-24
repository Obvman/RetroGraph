export module Rendering.GLShader;

import Utils;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

// The executable path is different whether we start the program from the
// visual studio or debugger, so this handles the two cases
const std::string shaderPath{ getExePath() + R"(\..\..\RetroGraph\Resources\shaders\)" };

export class [[nodiscard]] GLShaderBindScope {
public:
    GLShaderBindScope(GLuint id) {
        glUseProgram(id);
    }
    ~GLShaderBindScope() {
        glUseProgram(0);
    }
    GLShaderBindScope(const GLShaderBindScope&) = delete;
    GLShaderBindScope& operator=(const GLShaderBindScope&) = delete;
    GLShaderBindScope(GLShaderBindScope&&) = delete;
    GLShaderBindScope& operator=(GLShaderBindScope&&) = delete;
};

/* Compiles and links the given vertex and fragment shaders returns the ID
   of the compiled program if successful, otherwise presents a fatal
   message box with the GLSL compilation error message */
export class GLShader {
public:
    GLShader(const std::string& vertFilename_, const std::string& fragFilename_)
        : id{ loadShader(vertFilename_, fragFilename_) }
        , vertFilename{ vertFilename_ }
        , fragFilename{ fragFilename_ } { }
    GLShader(const std::string& baseName)
        : GLShader{ baseName + ".vert", baseName + ".frag" } { }
    ~GLShader();

    operator bool() const { return id > 0; }

    void reload();
    GLShaderBindScope bind() const { return { id }; }
    GLuint getUniformLocation(const char* uniformName) const { return glGetUniformLocation(id, uniformName); }

private:
    GLuint loadShader(const std::string& vFile, const std::string& fFile);
    std::string readShaderFile(const std::string& filePath) const;

    GLuint id;
    std::string vertFilename;
    std::string fragFilename;
};

} // namespace rg
