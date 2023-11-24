module Rendering.VBOController;

import Colors;
import UserSettings;

import Rendering.DrawUtils;

import "RGAssert.h";
import "GLHeaderUnit.h";

namespace rg {

// Automatically binds/unbinds given VBOs and executes the function given
void vboElemArrayDrawScope(const VBO<GLfloat>& vertVBO, const VBO<GLuint>& indexVBO, std::regular_invocable auto f) {
    auto vertsVBOScope{ vertVBO.bind() };
    auto indicesVBOScope{ indexVBO.bind() };
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    f();

    glDisableClientState(GL_VERTEX_ARRAY);
}

void vboDrawScope(const VBO<glm::vec2>& vbo, std::regular_invocable auto f) {
    auto vboScope{ vbo.bind() };
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    f();

    glDisableClientState(GL_VERTEX_ARRAY);
}

VBOController::VBOController() {
    initVBOs();
}

VBOController::~VBOController() {
}

void VBOController::reloadShaders() {
    m_animShader.reload();
}

void VBOController::initVBOs() {
    initGraphGridVBO();
}

void VBOController::initGraphGridVBO() {
    constexpr auto numVertLines = size_t{ 14U };
    constexpr auto numHorizLines = size_t{ 7U };

    auto& verts{ m_graphGridVerts.data };
    verts.reserve(4 * (numVertLines + numHorizLines));
    auto& indices{ m_graphGridIndices.data };
    indices.reserve(2 * (numVertLines + numHorizLines));

    // Fill the vertex and index arrays with data for drawing grid as VBO
    for (unsigned int i = 0U; i < numVertLines; ++i) {
        const float x{ (i) / static_cast<float>(numVertLines - 1) * 2.0f - 1.0f };
        verts.push_back(x);
        verts.push_back(1.0f); // Vertical line top vert

        verts.push_back(x);
        verts.push_back(-1.0f); // Vertical line bottom vert

        indices.push_back(2 * i);
        indices.push_back(2 * i + 1);
    }

    // Offset value for the index array
    const auto vertLineIndexCount{ static_cast<unsigned int>(indices.size()) };
    for (unsigned int i = 0U; i < numHorizLines; ++i) {
        const float y{ static_cast<float>(i) / (numHorizLines - 1) * 2.0f - 1.0f };
        verts.push_back(-1.0f);
        verts.push_back(y); // Horizontal line left vert

        verts.push_back(1.0f);
        verts.push_back(y); // Horizontal line right vert

        indices.push_back(vertLineIndexCount + 2 * i);
        indices.push_back(vertLineIndexCount + 2 * i + 1);
    }

    // Initialise the graph grid VBO
    auto vertsVBOScope{ m_graphGridVerts.bind() };
    glBufferData(GL_ARRAY_BUFFER, m_graphGridVerts.sizeBytes(), verts.data(), GL_STATIC_DRAW);

    // Initialise graph grid index array
    auto indicesVBOScope{ m_graphGridIndices.bind() };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_graphGridIndices.sizeBytes(), indices.data(), GL_STATIC_DRAW);
}

void VBOController::drawGraphGrid() const {
    vboElemArrayDrawScope(m_graphGridVerts, m_graphGridIndices, [this]() {
        glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.2f);
        glLineWidth(0.5f);
        glDrawElements(GL_LINES, m_graphGridIndices.size(), GL_UNSIGNED_INT, nullptr);
    });
}

void VBOController::drawGraphLines(VBOID vboId) const {
    const VBO<glm::vec2>& vbo{ getVBO(vboId) };
    vboDrawScope(vbo, [this, &vbo]() {
        glDrawArrays(GL_LINE_STRIP, 0, vbo.size());
    });
}

void VBOController::drawAnimationVBO(int size, float aspectRatio) const {
    auto shaderScope{ m_animShader.bind() };
    auto vaoScope{ m_animVAO.bind() };

    glm::vec3 const scale{ glm::vec3(1.0f, aspectRatio, 1.0f) };
    glm::mat4 modelMatrix{ glm::mat4() };
    modelMatrix = glm::scale(modelMatrix, scale);
    GLuint const location = m_animShader.getUniformLocation("model");
    glUniformMatrix4fv(location, 1, false, glm::value_ptr(modelMatrix));

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(size));
}

VBOID VBOController::createGraphLineVBO(size_t numValues) {
    m_vbos.emplace_back(static_cast<GLsizei>(numValues), GL_ARRAY_BUFFER);
    VBO<glm::vec2>& vbo{ m_vbos.back() };

    auto vboScope{ vbo.bind() };
    glBufferData(GL_ARRAY_BUFFER, vbo.sizeBytes(), vbo.data.data(), GL_STREAM_DRAW);

    return m_vbos.size() - 1;
}

void VBOController::createAnimationVBO(size_t numLines) {
    constexpr GLuint vertexLocationIndex{ 0 };
    constexpr GLuint lineLengthLocationIndex{ 1 };

    auto vaoScope{ m_animVAO.bind() };

    m_animLines.resize(numLines);
    auto vboScope{ m_animLines.bind() };

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(vertexLocationIndex, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(lineLengthLocationIndex, 1, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(2 * sizeof(GLfloat)));

    glBufferData(GL_ARRAY_BUFFER, m_animLines.sizeBytes(), m_animLines.data.data(), GL_STREAM_DRAW);
}

void VBOController::updateGraphLines(VBOID vboId, const std::vector<GLfloat>& values) {
    VBO<glm::vec2>& vbo{ getVBO(vboId) };
    auto& verts{ vbo.data };
    auto vboScope{ vbo.bind() };

    // Value vectors can change size (infrequently).
    // In this case we need to allocate buffer data instead of just writing to it
    if (vbo.size() != values.size()) {
        verts.resize(values.size());

        for (size_t i = 0; i < values.size(); ++i) {
            verts[i] = { (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f,
                         values[i] * 2.0f - 1.0f };
        }

        glBufferData(GL_ARRAY_BUFFER, vbo.sizeBytes(), verts.data(), GL_STREAM_DRAW);
    } else {
        for (size_t i = 0; i < values.size(); ++i) {
            verts[i] = { (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f,
                         values[i] * 2.0f - 1.0f };
        }

        glBufferSubData(GL_ARRAY_BUFFER, 0, vbo.sizeBytes(), verts.data());
    }
}

void VBOController::updateAnimationVBO(const AnimationState & as) {
    const auto numLines{ as.getNumLines() };

    auto& verts{ m_animLines.data };
    for (int i = 0; i < numLines; ++i) {
        verts[i] = as.getLines()[i];
    }

    auto vaoScope{ m_animVAO.bind() };
    auto vboScope{ m_animLines.bind() };

    glBufferSubData(GL_ARRAY_BUFFER, 0, numLines * m_animLines.elemBytes(), verts.data());
}

VBO<glm::vec2>& VBOController::getVBO(VBOID vboId) {
    return m_vbos[vboId];
}

const VBO<glm::vec2>& VBOController::getVBO(VBOID vboId) const {
    return m_vbos[vboId];
}

} // namespace rg
