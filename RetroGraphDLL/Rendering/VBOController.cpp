module Rendering.VBOController;

import Colors;
import UserSettings;

import "RGAssert.h";
import "GLHeaderUnit.h";

namespace rg {

// Automatically binds/unbinds given VBOs and executes the function given
void vboElemArrayDrawScope(GLuint vertID, GLuint indexID, std::regular_invocable auto f) {
    glBindBuffer(GL_ARRAY_BUFFER, vertID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    f();

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void vboDrawScope(GLuint vertID, std::regular_invocable auto f) {
    glBindBuffer(GL_ARRAY_BUFFER, vertID);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    f();

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VBOController::VBOController() {
    glGenBuffers(1, &m_graphGridVertsID);
    glGenBuffers(1, &m_graphGridIndicesID);

    initVBOs();
}

VBOController::~VBOController() {
    glDeleteBuffers(1, &m_graphGridVertsID);
    glDeleteVertexArrays(1, &m_graphGridIndicesID);
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
    glBindBuffer(GL_ARRAY_BUFFER, m_graphGridVertsID);
    glBufferData(GL_ARRAY_BUFFER, gVerts.size() * sizeof(GLfloat),
                 gVerts.data(), GL_STATIC_DRAW);

    // Initialise graph grid index array
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

void VBOController::drawGraphLines(VBOID vboId) const {
    const VBO& vbo{ getVBO(vboId) };
    vboDrawScope(vbo.id, [this, &vbo]() {
        glDrawArrays(GL_LINE_STRIP, 0, vbo.size);
    });
}

void VBOController::drawAnimationVBO(VBOID vboId, int size) const {
    // TODO colors!
    const VBO& vbo{ getVBO(vboId) };
    glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
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
    m_vbos.emplace_back(static_cast<GLsizei>(numValues));
    VBO& vbo{ m_vbos.back() };
    auto& verts{ vbo.data };

    glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_DYNAMIC_DRAW);

    return m_vbos.size() - 1;
}

VBOID VBOController::createAnimationVBO(size_t numLines) {
    // Each line has 2 vertices...
    m_vbos.emplace_back(static_cast<GLsizei>(numLines * 2));
    VBO& vbo{ m_vbos.back() };
    auto& verts{ vbo.data};

    // TODO create color buffer

    glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_DYNAMIC_DRAW);

    return m_vbos.size() - 1;
}

void VBOController::updateGraphLines(VBOID vboId, const std::vector<GLfloat>& values) {
    VBO& vbo{ getVBO(vboId) };
    auto& verts{ vbo.data };

    // Value vectors can change size. In this case we need to allocate buffer data instead of just writing to it
    if (vbo.size != values.size()) {
        vbo.size = static_cast<GLsizei>(values.size());
        verts.resize(vbo.size * 2);

        for (size_t i = 0; i < values.size(); ++i) {
            verts[2 * i] = (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f;
            verts[2 * i + 1] = values[i] * 2.0f - 1.0f;
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_DYNAMIC_DRAW);
    } else {
        for (size_t i = 0; i < values.size(); ++i) {
            verts[2 * i] = (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f;
            verts[2 * i + 1] = values[i] * 2.0f - 1.0f;
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(GLfloat), verts.data());
    }
}

void VBOController::updateAnimationVBO(VBOID vboId, const AnimationState & as) {
    VBO& vbo{ getVBO(vboId) };
    auto& verts{ vbo.data };

    const auto numLines{ as.getNumLines() };

    for (int i = 0; i < numLines; ++i) {
        const auto& line{ as.getLines()[i] };
        verts[4 * i]     = line.x1;
        verts[4 * i + 1] = line.y1;
        verts[4 * i + 2] = line.x2;
        verts[4 * i + 3] = line.y2;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numLines * sizeof(ParticleLine), verts.data());
}

VBO& VBOController::getVBO(VBOID vboId) {
    return m_vbos[vboId];
}

const VBO& VBOController::getVBO(VBOID vboId) const {
    return m_vbos[vboId];
}

} // namespace rg
