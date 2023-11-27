module Widgets.LineGraph;

import Rendering.GLListContainer;

import "GLHeaderUnit.h";

namespace rg {

constexpr auto numGridVertLines = size_t{ 14U };
constexpr auto numGridHorizLines = size_t{ 7U };
constexpr auto numGridVerts = size_t{ 2 * (numGridVertLines + numGridHorizLines) };

LineGraph::LineGraph(size_t numGraphSamples, const glm::vec4& color, bool drawBackground)
    : m_graphPointsVBO{ static_cast<GLsizei>(numGraphSamples), GL_ARRAY_BUFFER, GL_STREAM_DRAW }
    , m_graphGridVerts{ static_cast<GLsizei>(numGridVerts), GL_ARRAY_BUFFER, GL_STATIC_DRAW }
    , m_graphGridIndices{ static_cast<GLsizei>(numGridVerts), GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW }
    , m_color{ color }
    , m_drawBackground{ drawBackground } {

    if (m_drawBackground)
        initGridVBO();

    initPointsVBO();
}

void LineGraph::draw() const {
    if (m_drawBackground) {
        GLListContainer::inst().drawBorder();
        drawGrid();
    }

    drawPoints();
}

void LineGraph::updatePoints(const std::vector<GLfloat>& values) {
    auto& verts{ m_graphPointsVBO.data()};
    auto vboScope{ m_graphPointsVBO.bind() };

    // Value vectors can change size (infrequently).
    // In this case we need to allocate buffer data instead of just writing to it
    if (m_graphPointsVBO.size() != values.size()) {
        verts.resize(values.size());

        for (size_t i = 0; i < values.size(); ++i) {
            verts[i] = { (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f,
                         values[i] * 2.0f - 1.0f };
        }

        m_graphPointsVBO.bufferData();
    } else {
        for (size_t i = 0; i < values.size(); ++i) {
            verts[i] = { (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f,
                         values[i] * 2.0f - 1.0f };
        }

        m_graphPointsVBO.bufferSubData(0, m_graphPointsVBO.sizeBytes());
    }
}

void LineGraph::initGridVBO() {
    auto& verts{ m_graphGridVerts.data()};
    auto& indices{ m_graphGridIndices.data()};

    // Fill the vertex and index arrays with data for drawing grid as VBO
    for (unsigned int i = 0U; i < numGridVertLines; ++i) {
        const float x{ (i) / static_cast<float>(numGridVertLines - 1) * 2.0f - 1.0f };
        verts[2 * i]     = { x, 1.0f }; // Vertical line top vert
        verts[2 * i + 1] = { x, -1.0f }; // Vertical line bottom vert

        indices[2 * i] = 2 * i;
        indices[2 * i + 1] = 2 * i + 1;
    }

    // Offset value for the index array
    const auto vertLineIndexCount{ numGridVertLines * 2 };
    for (unsigned int i = 0U; i < numGridHorizLines; ++i) {
        const float y{ static_cast<float>(i) / (numGridHorizLines - 1) * 2.0f - 1.0f };
        verts[2 * (i + numGridVertLines)]     = { -1.0f, y }; // Horizontal line left vert
        verts[2 * (i + numGridVertLines) + 1] = { 1.0f, y }; // Horizontal line right vert

        indices[2 * (i + numGridVertLines)] = vertLineIndexCount + 2 * i;
        indices[2 * (i + numGridVertLines) + 1] = vertLineIndexCount + 2 * i + 1;
    }

    // Initialise the graph grid VBO
    auto vertsVBOScope{ m_graphGridVerts.bind() };
    m_graphGridVerts.bufferData();

    // Initialise graph grid index array
    auto indicesVBOScope{ m_graphGridIndices.bind() };
    m_graphGridIndices.bufferData();
}

void LineGraph::initPointsVBO() {
    auto vboScope{ m_graphPointsVBO.bind() };
    m_graphPointsVBO.bufferData();
}

void LineGraph::drawGrid() const {
    auto vertsVBOScope{ m_graphGridVerts.bind() };
    auto indicesVBOScope{ m_graphGridIndices.bind() };

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    glColor4f(m_color.x, m_color.y, m_color.z, 0.2f);
    glLineWidth(0.5f);
    glDrawElements(GL_LINES, m_graphGridIndices.size(), GL_UNSIGNED_INT, nullptr);

    glDisableClientState(GL_VERTEX_ARRAY);
}

void LineGraph::drawPoints() const {
    glColor4f(m_color.x, m_color.y, m_color.z, m_color.w);

    auto vboScope{ m_graphPointsVBO.bind() };
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    glDrawArrays(GL_LINE_STRIP, 0, m_graphPointsVBO.size());

    glDisableClientState(GL_VERTEX_ARRAY);
}

}
