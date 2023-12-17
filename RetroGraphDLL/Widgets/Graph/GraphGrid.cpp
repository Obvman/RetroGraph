module RG.Widgets.Graph:GraphGrid;

import Colors;

import RG.Rendering;

import "GLHeaderUnit.h";

namespace rg {

constexpr auto numGridVertLines = size_t{ 14U };
constexpr auto numGridHorizLines = size_t{ 7U };
constexpr auto numGridVerts = size_t{ 2 * (numGridVertLines + numGridHorizLines) };

GraphGrid::GraphGrid()
    : m_graphGridVerts{ static_cast<GLsizei>(numGridVerts), GL_ARRAY_BUFFER, GL_STATIC_DRAW }
    , m_graphGridIndices{ static_cast<GLsizei>(numGridVerts), GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW }
{
    initVBO();
}

void GraphGrid::draw() const {
    auto vertsVBOScope{ m_graphGridVerts.bind() };
    auto indicesVBOScope{ m_graphGridIndices.bind() };

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.2f);
    glLineWidth(0.5f);
    glDrawElements(GL_LINES, m_graphGridIndices.size(), GL_UNSIGNED_INT, nullptr);

    glDisableClientState(GL_VERTEX_ARRAY);
}

void GraphGrid::initVBO() {
    auto& verts{ m_graphGridVerts.data()};
    auto& indices{ m_graphGridIndices.data()};

    // Fill the vertex and index arrays with data for drawing grid as VBO
    for (unsigned int i = 0U; i < numGridVertLines; ++i) {
        const float x{ percentageToVP(i / static_cast<float>(numGridVertLines - 1)) };
        verts[2 * i]     = { x, viewportMax }; // Vertical line top vert
        verts[2 * i + 1] = { x, viewportMin }; // Vertical line bottom vert

        indices[2 * i] = 2 * i;
        indices[2 * i + 1] = 2 * i + 1;
    }

    const auto vertLineIndexCount{ numGridVertLines * 2 };
    for (unsigned int i = 0U; i < numGridHorizLines; ++i) {
        const float y{ percentageToVP(static_cast<float>(i) / (numGridHorizLines - 1)) };
        verts[2 * (i + numGridVertLines)]     = { viewportMin, y }; // Horizontal line left vert
        verts[2 * (i + numGridVertLines) + 1] = { viewportMax, y }; // Horizontal line right vert

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

}
