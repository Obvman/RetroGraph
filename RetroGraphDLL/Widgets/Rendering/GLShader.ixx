module;

#include "RGAssert.h"

export module GLShader;

import Utils;

import std.core;

import "GLHeaders.h";

namespace rg {

export { class GLShader; }

// The executable path is different whether we start the program from the
// visual studio or debugger, so this handles the two cases
const std::string shaderPath{ getExePath() + R"(\..\..\RetroGraph\Resources\shaders\)" };

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
    std::string readShaderFile(const std::string& filePath) const;

    GLuint id;
};


GLuint GLShader::loadShader(const std::string& vFile, const std::string& fFile) {
    const auto vShader{ glCreateShader(GL_VERTEX_SHADER) };
    const auto fShader{ glCreateShader(GL_FRAGMENT_SHADER) };

    // Read the shader source code
    std::string vertShaderStr{ readShaderFile(shaderPath + vFile) };
    std::string fragShaderStr{ readShaderFile(shaderPath + fFile) };
    const char* vertShaderSrc{ vertShaderStr.c_str() };
    const char* fragShaderSrc{ fragShaderStr.c_str() };

    // Compile the shaders
    glShaderSource(vShader, 1, &vertShaderSrc, nullptr);
    glCompileShader(vShader);

    // Check successful shader compilation
    GLint result{ GL_FALSE };
    int logLength{ 0 };
    std::string errorMessage{};

    glGetShaderiv(vShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> vertShaderError((logLength > 1) ? logLength : 1);
        glGetShaderInfoLog(vShader, logLength, nullptr, &vertShaderError[0]);

        errorMessage.append(&vertShaderError[0]);
        std::cout << &vertShaderError[0] << '\n';
    }

    glShaderSource(fShader, 1, &fragShaderSrc, nullptr);
    glCompileShader(fShader);

    // Check successful shader compilation
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> fragShaderError((logLength > 1) ? logLength : 1);
        glGetShaderInfoLog(fShader, logLength, nullptr, &fragShaderError[0]);

        errorMessage.append(&fragShaderError[0]);
        std::cout << &fragShaderError[0] << '\n';
    }

    GLuint program{ glCreateProgram() };
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);

    // Check linking success
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> programError((logLength > 1) ? logLength : 1);
        glGetProgramInfoLog(program, logLength, nullptr, &programError[0]);
        std::cout << &programError[0] << '\n';

        errorMessage.append(&programError[0]);
        RGERROR(errorMessage.c_str());

        glDeleteShader(vShader);
        glDeleteShader(fShader);

        return 0;
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);

    return program;
}

std::string GLShader::readShaderFile(const std::string& filePath) const {
    std::ifstream fileStream(filePath, std::ios::in);
    std::string fileContents{};

    RGASSERT(fileStream.is_open(), std::format("Failed to open shader file {}", filePath).c_str());

    std::string line{};
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        fileContents.append(line + "\n");
    }

    fileStream.close();
    return fileContents;
}

}
