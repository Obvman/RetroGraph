module Widgets.LineGraph;

import Rendering.DrawUtils;
import Rendering.GLListContainer;

import "GLHeaderUnit.h";

namespace rg {

LineGraph::LineGraph(size_t numGraphSamples)
    : m_graphGrid{}
    , m_graphPointsVBO{ static_cast<GLsizei>(numGraphSamples), GL_ARRAY_BUFFER, GL_STREAM_DRAW } {

    initPointsVBO();
}

void LineGraph::draw() const {
    GLListContainer::inst().drawBorder();
    m_graphGrid.draw();
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

void LineGraph::initPointsVBO() {
    auto vboScope{ m_graphPointsVBO.bind() };
    m_graphPointsVBO.bufferData();
}

void LineGraph::drawPoints() const {
    glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);

    auto vboScope{ m_graphPointsVBO.bind() };
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    glDrawArrays(GL_LINE_STRIP, 0, m_graphPointsVBO.size());

    glDisableClientState(GL_VERTEX_ARRAY);
}

}
