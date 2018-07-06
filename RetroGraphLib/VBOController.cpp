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
    // TODO glDeleteBuffers()
}

void VBOController::drawGraphGrid() const {
    vboElemArrayDrawScope(m_graphGridVertsID, m_graphGridIndicesID, [this]() {
        glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.2f);
        glLineWidth(0.5f);
        glDrawElements(GL_LINES, m_graphGridIndicesSize, GL_UNSIGNED_INT, nullptr);
    });
}

void VBOController::drawGraphLines(int vboID) const {
    vboDrawScope(m_graphLineVBOData[vboID].id, [this, vboID]() {
        glDrawArrays(GL_LINE_STRIP, 0, m_graphLineVBOData[vboID].size);
    });
}

int VBOController::createGraphLineVBO(GLsizei numValues) {
    std::vector<GLfloat> values( numValues, 0.0f );
    std::vector<GLfloat> verts( numValues * 2 );

    for (int i = 0; i < values.size(); ++i) {
        verts[2 * i]     = (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f;
        verts[2 * i + 1] = values[i] * 2.0f - 1.0f;
    }
    m_graphLineVBOData.emplace_back(numValues);

    glGenBuffers(1, &m_graphLineVBOData.back().id);
    glBindBuffer(GL_ARRAY_BUFFER, m_graphLineVBOData.back().id);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_DYNAMIC_DRAW);

    return m_graphLineVBOData.size() - 1;
}

void VBOController::updateGraphLines(int vboID, const std::vector<GLfloat>& values) const {
    // TODO cache verts so we don't need to allocate every time.

    std::vector<GLfloat> verts(values.size() * 2);
    for (int i = 0; i < values.size(); ++i) {
        verts[2 * i] = (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f;
        verts[2 * i + 1] = values[i] * 2.0f - 1.0f;
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_graphLineVBOData[vboID].id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(GLfloat), verts.data());
}

void VBOController::initVBOs() {
    const auto& settings{ UserSettings::inst() };

    initGraphGridVBO();

    // initGraphLineVBO(std::get<uint32_t>(settings.getVal("Widgets-Graphs-GPU.NumUsageSamples")));
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

}
