#pragma once

#include <GL/glew.h>
#include <string>

namespace rg {

/* Compiles, links and stores GLSL programs */
class GLShaderHandler {
public:
    GLShaderHandler();
    ~GLShaderHandler();

    void loadShaders();

    GLuint getTestProgram() const { return m_testProgram; }
    GLuint getCpuGraphProgram() const { return m_cpuGraphProgram; }
private:
    /* Returns the contents of the shader file given */
    std::string readShaderFile(const std::string& filePath);

    /* Compiles and links the given vertex and fragment shaders */
    GLuint loadShader(const std::string& vFile, const std::string& fFile);

    GLuint m_testProgram;
    GLuint m_cpuGraphProgram;
};

}