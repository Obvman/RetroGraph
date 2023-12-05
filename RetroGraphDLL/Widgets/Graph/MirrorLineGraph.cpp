module Widgets.Graph.MirrorLineGraph;

import Colors;

import Rendering.DrawUtils;
import Rendering.GLListContainer;

import Widgets.Graph.GraphGrid;
import Widgets.Graph.Spline;

import "GLHeaderUnit.h";

namespace rg {

MirrorLineGraph::MirrorLineGraph(size_t numTopGraphSamples, size_t numBottomGraphSamples)
    : m_topGraphPointsVBO{ static_cast<GLsizei>(numTopGraphSamples), GL_ARRAY_BUFFER, GL_STREAM_DRAW }
    , m_bottomGraphPointsVBO{ static_cast<GLsizei>(numBottomGraphSamples), GL_ARRAY_BUFFER, GL_STREAM_DRAW } {

    initPointsVBO(m_topGraphPointsVBO);
    initPointsVBO(m_bottomGraphPointsVBO);
}

void MirrorLineGraph::draw() const {
    GLListContainer::inst().drawBorder();
    GraphGrid::inst().draw();

    auto viewport{ getGLViewport() };

    // Draw the top graph in the top half of the viewport
    glViewport(viewport.x, viewport.y + (viewport.height / 2), viewport.width, viewport.height / 2);
    drawPoints(m_topGraphPointsVBO);

    // Draw the bottom graph mirrored in the bottom half of the viewport
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height / 2);
    glPushMatrix(); {
        glScalef(1.0, -1.0f, 1.0f);
        drawPoints(m_bottomGraphPointsVBO);
    } glPopMatrix();
}

void MirrorLineGraph::updatePoints(const std::vector<float>& topValues, const std::vector<float>& bottomValues) {
    updatePointsVBO(m_topGraphPointsVBO, topValues);
    updatePointsVBO(m_bottomGraphPointsVBO, bottomValues);
}

void MirrorLineGraph::initPointsVBO(OwningVBO<glm::vec2>& vbo) {
    auto vboScope{ vbo.bind() };
    vbo.bufferData();
}

void MirrorLineGraph::updatePointsVBO(OwningVBO<glm::vec2>& vbo, const std::vector<float>& values) {
    auto& verts{ vbo.data()};
    auto vboScope{ vbo.bind() };

    // Value vectors can change size (infrequently).
    // In this case we need to allocate buffer data instead of just writing to it
    if (vbo.size() != values.size()) {
        verts.resize(values.size());

        for (size_t i = 0; i < values.size(); ++i) {
            verts[i] = { percentageToVP(static_cast<float>(i) / (values.size() - 1)),
                         percentageToVP(values[i]) };
        }

        vbo.bufferData();
    } else {
        for (size_t i = 0; i < values.size(); ++i) {
            verts[i] = { percentageToVP(static_cast<float>(i) / (values.size() - 1)),
                         percentageToVP(values[i]) };
        }

        vbo.bufferSubData(0, vbo.sizeBytes());
    }
}

void MirrorLineGraph::drawPoints(const OwningVBO<glm::vec2>& vbo) const {
    glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);

    auto vboScope{ vbo.bind() };
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    glDrawArrays(GL_LINE_STRIP, 0, vbo.size());

    glDisableClientState(GL_VERTEX_ARRAY);
}

SmoothMirrorLineGraph::SmoothMirrorLineGraph(size_t precisionPoints)
    : MirrorLineGraph{ precisionPoints, precisionPoints }
    , m_precisionPoints{ precisionPoints } {

}

void SmoothMirrorLineGraph::updatePointsVBO(OwningVBO<glm::vec2>& vbo, const std::vector<float>& values) {
    auto& verts{ vbo.data()};
    auto vboScope{ vbo.bind() };

    std::vector<float> rawX (values.size());
    std::vector<float> rawY (values.size());
    for (size_t i = 0; i < values.size(); ++i) {
        rawX[i] = percentageToVP(static_cast<float>(i) / (values.size() - 1));
        rawY[i] = percentageToVP(values[i]);
    }

    tk::spline spl(rawX, rawY, tk::spline::cspline_hermite);
    for (size_t i = 0; i < m_precisionPoints; ++i) {
        float const x{ percentageToVP(static_cast<float>(i) / m_precisionPoints) };
        verts[i] = { x, clampToViewport(spl(x)) };
    }

    // #TODO sliding buffer so we don't need to copy/reallocate every update.
    vbo.bufferSubData(0, vbo.sizeBytes());
}

}
