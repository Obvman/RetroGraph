export module RG.Rendering:Shader;

import Utils;

import RG.Core;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export using ShaderRefreshEvent = CallbackEvent<>;

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
export class Shader {
public:
    Shader(const std::string& vertFilename_, const std::string& fragFilename_);
    Shader(const std::string& baseName)
        : Shader{ baseName + ".vert", baseName + ".frag" } { }
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) = delete;
    Shader& operator=(Shader&& other) = delete;

    GLShaderBindScope bind() const { return { m_id }; }
    GLuint getUniformLocation(const char* uniformName) const { return glGetUniformLocation(m_id, uniformName); }

    static void requestShaderRefresh() { onRefreshRequested.raise(); }

    // Raised after the shader gets reloaded
    ShaderRefreshEvent onRefresh;

private:
    GLuint loadShader(const std::string& vFile, const std::string& fFile);
    std::string readShaderFile(const std::string& filePath) const;
    void reload();

    GLuint m_id;
    std::string m_vertFilename;
    std::string m_fragFilename;
    ShaderRefreshEvent::Handle m_onRefreshRequestedHandle;

    static inline ShaderRefreshEvent onRefreshRequested;
};

} // namespace rg
