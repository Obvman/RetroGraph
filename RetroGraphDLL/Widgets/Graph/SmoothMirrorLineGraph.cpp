module Widgets.Graph.SmoothMirrorLineGraph;

import Colors;

import Rendering.DrawUtils;
import Rendering.GLListContainer;

import Widgets.Graph.GraphGrid;
import Widgets.Graph.Spline;

import "GLHeaderUnit.h";

namespace rg {

SmoothMirrorLineGraph::SmoothMirrorLineGraph(size_t numTopGraphSamples, size_t numBottomGraphSamples)
    : m_topGraph{numTopGraphSamples}
    , m_bottomGraph{ numBottomGraphSamples } {

    glm::mat4 verticalInversionMatrix{};
    m_bottomGraph.setModelView(glm::scale(verticalInversionMatrix, { 1.0f, -1.0f, 1.0f }));
    m_bottomGraph.setDrawDecorations(false);
    m_topGraph.setDrawDecorations(false);
}

void SmoothMirrorLineGraph::updatePoints(const std::vector<float>& topValues, const std::vector<float>& bottomValues) {
    m_topGraph.updatePoints(topValues);
    m_bottomGraph.updatePoints(bottomValues);
}

void SmoothMirrorLineGraph::resetPoints(const std::vector<float>& topValues, const std::vector<float>& bottomValues) {
    m_topGraph.resetPoints(topValues);
    m_bottomGraph.resetPoints(bottomValues);
}

void SmoothMirrorLineGraph::draw() const {
    GLListContainer::inst().drawBorder();
    GraphGrid::inst().draw();

    // Draw a dividing line between the two graphs
    glBegin(GL_LINES); {
        glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.3f);
        glVertex2f(-1.0f, 0.0f);
        glVertex2f(1.0f, 0.0f);
    } glEnd();

    auto viewport{ getGLViewport() };

    // Draw the top graph in the top half of the viewport
    glViewport(viewport.x, viewport.y + (viewport.height / 2), viewport.width, viewport.height / 2);
    m_topGraph.draw();

    // Draw the bottom graph mirrored in the bottom half of the viewport
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height / 2);
    m_bottomGraph.draw();
}

}
