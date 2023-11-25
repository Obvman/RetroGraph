module Widgets.LineGraph;

import Rendering.GLListContainer;

import "GLHeaderUnit.h";

namespace rg {

LineGraph::LineGraph(size_t numGraphSamples, const glm::vec4& color, bool drawBackground)
    : m_graphPointsVBO{ static_cast<GLsizei>(numGraphSamples), GL_ARRAY_BUFFER }
    , m_graphGridVerts{ GL_ARRAY_BUFFER }
    , m_graphGridIndices{ GL_ELEMENT_ARRAY_BUFFER }
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

void LineGraph::updatePoints(const std::vector<GLfloat>& values) const {
    auto& verts{ m_graphPointsVBO.data };
    auto vboScope{ m_graphPointsVBO.bind() };

    // Value vectors can change size (infrequently).
    // In this case we need to allocate buffer data instead of just writing to it
    if (m_graphPointsVBO.size() != values.size()) {
        verts.resize(values.size());

        for (size_t i = 0; i < values.size(); ++i) {
            verts[i] = { (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f,
                         values[i] * 2.0f - 1.0f };
        }

        glBufferData(GL_ARRAY_BUFFER, m_graphPointsVBO.sizeBytes(), verts.data(), GL_STREAM_DRAW);
    } else {
        for (size_t i = 0; i < values.size(); ++i) {
            verts[i] = { (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f,
                         values[i] * 2.0f - 1.0f };
        }

        glBufferSubData(GL_ARRAY_BUFFER, 0, m_graphPointsVBO.sizeBytes(), verts.data());
    }
}

void LineGraph::initGridVBO() {
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

void LineGraph::initPointsVBO() {
    auto vboScope{ m_graphPointsVBO.bind() };
    glBufferData(GL_ARRAY_BUFFER, m_graphPointsVBO.sizeBytes(), m_graphPointsVBO.data.data(), GL_STREAM_DRAW);
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
