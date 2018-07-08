#include "stdafx.h"
#include "VBOController.h"

#include "colors.h"
#include "UserSettings.h"

#include <variant>

namespace rg {

VBOController::VBOController()
    : m_graphLineVBOData{} {

    initVBOs();
}


VBOController::~VBOController() {
    glDeleteBuffers(1, &m_graphGridVertsID);
    glDeleteVertexArrays(1, &m_graphGridIndicesID);

    for (const auto& vbo : m_graphLineVBOData)
        if (vbo.id != UINT_MAX)
            glDeleteBuffers(1, &vbo.id);
}

void VBOController::drawGraphGrid() const {
    vboElemArrayDrawScope(m_graphGridVertsID, m_graphGridIndicesID, [this]() {
        glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.2f);
        glLineWidth(0.5f);
        glDrawElements(GL_LINES, m_graphGridIndicesSize, GL_UNSIGNED_INT, nullptr);
    });
}

void VBOController::drawGraphLines(const VBOID& vboID) const {
    vboDrawScope(m_graphLineVBOData[static_cast<int>(vboID)].id, [this, &vboID]() {
        glDrawArrays(GL_LINE_STRIP, 0, m_graphLineVBOData[static_cast<int>(vboID)].size);
    });
}

VBOID VBOController::createGraphLineVBO(size_t numValues) {
    m_graphLineVBOData.emplace_back(static_cast<GLsizei>(numValues));

    auto& vboContainer{ m_graphLineVBOData.back() };
    auto& verts{ vboContainer.data };
    std::vector<GLfloat> values(numValues, 0.0f);

    for (int i = 0; i < values.size(); ++i) {
        verts[2 * i] = (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f;
        verts[2 * i + 1] = values[i] * 2.0f - 1.0f;
    }

    glGenBuffers(1, &vboContainer.id);
    glBindBuffer(GL_ARRAY_BUFFER, vboContainer.id);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_DYNAMIC_DRAW);

    return VBOID{ m_graphLineVBOData.size() - 1 };
}

VBOID VBOController::createVBO(size_t numValues) {
    m_graphLineVBOData.emplace_back(static_cast<GLsizei>(numValues));

    return VBOID{ m_graphLineVBOData.size() - 1 };
}

void VBOController::destroyVBO(const VBOID& vboIdx) {
    if (!vboIdx)
        return;

    auto& container{ m_graphLineVBOData[static_cast<int>(vboIdx)] };

    if (container.id != UINT_MAX)
        glDeleteBuffers(1, &container.id);

    // We can't shift the elements of the container around, since outside
    // resources point to indexes. So we 'destroy' it by copying to an empty container
    container = VBOContainer{}; // Clear resources
    container.id = UINT_MAX;
}

void VBOController::updateGraphLines(const VBOID& vboID, const std::vector<GLfloat>& values) {
    auto& vboContainer{ m_graphLineVBOData[static_cast<int>(vboID)] };
    auto& verts{ vboContainer.data };

    for (int i = 0; i < values.size(); ++i) {
        verts[2 * i] = (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f;
        verts[2 * i + 1] = values[i] * 2.0f - 1.0f;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vboContainer.id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(GLfloat), verts.data());
}

void VBOController::initVBOs() {
    initGraphGridVBO();
}

void VBOController::initGraphGridVBO() {
    constexpr auto numVertLines = size_t{ 14U };
    constexpr auto numHorizLines = size_t{ 7U };

    auto gVerts = std::vector<GLfloat>{};
    auto gIndices = std::vector<GLuint>{};
    gVerts.reserve(4 * (numVertLines + numHorizLines));
    gIndices.reserve(2 * (numVertLines + numHorizLines));

    // Fill the vertex and index arrays with data for drawing grid as VBO
    for (unsigned int i = 0U; i < numVertLines; ++i) {
        const float x{ (i) / static_cast<float>(numVertLines - 1) * 2.0f - 1.0f };
        gVerts.push_back(x);
        gVerts.push_back(1.0f); // Vertical line top vert

        gVerts.push_back(x);
        gVerts.push_back(-1.0f); // Vertical line bottom vert

        gIndices.push_back(2 * i);
        gIndices.push_back(2 * i + 1);
    }

    // Offset value for the index array
    const auto vertLineIndexCount{ static_cast<unsigned int>(gIndices.size()) };
    for (unsigned int i = 0U; i < numHorizLines; ++i) {
        const float y{ static_cast<float>(i) / (numHorizLines - 1) * 2.0f - 1.0f };
        gVerts.push_back(-1.0f);
        gVerts.push_back(y); // Horizontal line bottom vert

        gVerts.push_back(1.0f);
        gVerts.push_back(y); // Horizontal line top vert

        gIndices.push_back(vertLineIndexCount + 2 * i);
        gIndices.push_back(vertLineIndexCount + 2 * i + 1);
    }
    m_graphGridIndicesSize = vertLineIndexCount;


    // Initialise the graph grid VBO
    glGenBuffers(1, &m_graphGridVertsID);
    glBindBuffer(GL_ARRAY_BUFFER, m_graphGridVertsID);
    glBufferData(GL_ARRAY_BUFFER, gVerts.size() * sizeof(GLfloat),
                 gVerts.data(), GL_STATIC_DRAW);

    // Initialise graph grid index array
    glGenBuffers(1, &m_graphGridIndicesID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_graphGridIndicesID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gIndices.size() * sizeof(GLuint),
                 gIndices.data(), GL_STATIC_DRAW);

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

VBOID::VBOID(VBOID&& other) {
    m_id = other.m_id;
    other.m_id = -1;
}

VBOID& VBOID::operator=(VBOID&& other) {
    if (&other != this) {
        m_id = other.m_id;
        other.m_id = -1;
    }
    return *this;
}

}
