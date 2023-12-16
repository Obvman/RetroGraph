module RG.Rendering:Shader;

import "GLHeaderUnit.h";
import "RGAssert.h";

namespace rg {

Shader::Shader(const std::string& vertFilename_, const std::string& fragFilename_)
    : m_id{ loadShader(vertFilename_, fragFilename_) }
    , m_vertFilename{ vertFilename_ }
    , m_fragFilename{ fragFilename_ }
    , m_onRefreshRequestedHandle{ Shader::onRefreshRequested.attach([this]() { reload(); }) } {
}

Shader::~Shader() {
    if (m_id > 0)
        glDeleteProgram(m_id);

    Shader::onRefreshRequested.detach(m_onRefreshRequestedHandle);
}

void Shader::reload() {
    if (m_id > 0)
        glDeleteProgram(m_id);

    loadShader(m_vertFilename, m_fragFilename);

    onRefresh.raise();
}

GLuint Shader::loadShader(const std::string& vFile, const std::string& fFile) {
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
        RGERROR(errorMessage.c_str());
        return 0;
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
        RGERROR(errorMessage.c_str());
        return 0;
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

std::string Shader::readShaderFile(const std::string& filePath) const {
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

} // namespace rg
