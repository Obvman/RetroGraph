#include "stdafx.h"
#include "VBOController.h"

#include "colors.h"
#include "UserSettings.h"
#include "AnimationState.h"
#include "GLShaders.h"

#include <variant>

namespace rg {

VBOController::VBOController()
    : m_graphLineVBOData{}
    , m_particleLineShader{ "test" } {

    if (!m_particleLineShader) {
        fatalMessageBox("Failed to init shader\n");
    }

    initVBOs();
}


VBOController::~VBOController() {
    glDeleteBuffers(1, &m_graphGridVertsID);
    glDeleteVertexArrays(1, &m_graphGridIndicesID);

    for (const auto& vbo : m_graphLineVBOData)
        if (vbo.id != UINT_MAX)
            glDeleteBuffers(1, &vbo.id);
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
        gVerts.push_back(y); // Horizontal line left vert

        gVerts.push_back(1.0f);
        gVerts.push_back(y); // Horizontal line right vert

        gIndices.push_back(vertLineIndexCount + 2 * i);
        gIndices.push_back(vertLineIndexCount + 2 * i + 1);
    }
    m_graphGridIndicesSize = static_cast<GLsizei>(gIndices.size());


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

void VBOController::drawAnimationVBO(const VBOID & vboID, int size) const {
    // TODO colors!
    glBindBuffer(GL_ARRAY_BUFFER, m_graphLineVBOData[static_cast<int>(vboID)].id);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    //glBindBuffer(GL_ARRAY_BUFFER, m_animColors);
    //glVertexAttribPointer(1U, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    //glEnableVertexAttribArray(1U);

    //glUseProgram(particleLineShader);

    //glColor4f(PARTICLE_R, PARTICLE_G, PARTICLE_B, 1.0f);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(size));

    //glUseProgram(0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VBOID VBOController::createGraphLineVBO(size_t numValues) {
    auto& vboContainer{ m_graphLineVBOData.emplace_back(static_cast<GLsizei>(numValues)) };
    auto& verts{ vboContainer.data };
    std::vector<GLfloat> values(numValues, 0.0f);

    for (size_t i = 0; i < values.size(); ++i) {
        verts[2 * i] = (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f;
        verts[2 * i + 1] = values[i] * 2.0f - 1.0f;
    }

    glGenBuffers(1, &vboContainer.id);
    glBindBuffer(GL_ARRAY_BUFFER, vboContainer.id);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_DYNAMIC_DRAW);

    return VBOID{ m_graphLineVBOData.size() - 1 };
}

VBOID VBOController::createAnimationVBO(size_t numLines) {
    // Each line has 2 vertices...
    auto& vboContainer{ m_graphLineVBOData.emplace_back(static_cast<GLsizei>(numLines * 2)) };
    auto& verts{ vboContainer.data };

    // TODO create color buffer

    glGenBuffers(1, &vboContainer.id);
    glBindBuffer(GL_ARRAY_BUFFER, vboContainer.id);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_DYNAMIC_DRAW);

    //std::vector<GLfloat> colors(verts.size() * 2, 1.0f); // 4 color components vs 2 coords = double size
    //glGenBuffers(1, &m_animColors);
    //glBindBuffer(GL_ARRAY_BUFFER, m_animColors);
    //glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), GL_DYNAMIC_DRAW);

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

    // Value vectors can change size. In this case we only to allocate buffer data instead of just writing to it
    if (vboContainer.size != values.size()) {
        vboContainer.size = static_cast<GLsizei>(values.size());
        verts.resize(vboContainer.size * 2);

        for (size_t i = 0; i < values.size(); ++i) {
            verts[2 * i] = (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f;
            verts[2 * i + 1] = values[i] * 2.0f - 1.0f;
        }

        glBindBuffer(GL_ARRAY_BUFFER, vboContainer.id);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_DYNAMIC_DRAW);
    } else {
        for (size_t i = 0; i < values.size(); ++i) {
            verts[2 * i] = (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f;
            verts[2 * i + 1] = values[i] * 2.0f - 1.0f;
        }

        glBindBuffer(GL_ARRAY_BUFFER, vboContainer.id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(GLfloat), verts.data());
    }
}

void VBOController::updateAnimationVBO(const VBOID& vboID, const AnimationState & as) {
    auto& vboContainer{ m_graphLineVBOData[static_cast<int>(vboID)] };
    auto& verts{ vboContainer.data };

    const auto numLines{ as.getNumLines() };

    for (int i = 0; i < numLines; ++i) {
        const auto& line{ as.getLines()[i] };
        verts[4 * i]     = line.x1;
        verts[4 * i + 1] = line.y1;
        verts[4 * i + 2] = line.x2;
        verts[4 * i + 3] = line.y2;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vboContainer.id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numLines * sizeof(ParticleLine), verts.data());
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
