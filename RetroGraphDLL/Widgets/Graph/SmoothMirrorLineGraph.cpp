module RG.Widgets.Graph:SmoothMirrorLineGraph;

import :GraphGrid;
import :Spline;

import Colors;

import RG.Rendering;

import "GLHeaderUnit.h";
import "RGAssert.h";

namespace rg {

SmoothMirrorLineGraph::SmoothMirrorLineGraph(size_t numGraphSamples)
    : m_topGraph{ numGraphSamples }
    , m_bottomGraph{ numGraphSamples } {
    glm::mat4 verticalInversionMatrix{};
    m_bottomGraph.setModelView(glm::scale(verticalInversionMatrix, { 1.0f, -1.0f, 1.0f }));
    m_bottomGraph.setDrawDecorations(false);
    m_topGraph.setDrawDecorations(false);
}

void SmoothMirrorLineGraph::resetPoints(size_t numGraphSamples) {
    m_topGraph.resetPoints(numGraphSamples);
    m_bottomGraph.resetPoints(numGraphSamples);
}

void SmoothMirrorLineGraph::draw() const {
    GLListContainer::inst().drawBorder();
    GraphGrid::inst().draw();

    // Draw a dividing line between the two graphs
    glBegin(GL_LINES);
    {
        glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.3f);
        glVertex2f(-1.0f, 0.0f);
        glVertex2f(1.0f, 0.0f);
    }
    glEnd();

    auto viewport{ getGLViewport() };

    // Draw the top graph in the top half of the viewport
    glViewport(viewport.x, viewport.y + (viewport.height / 2), viewport.width, viewport.height / 2);
    m_topGraph.draw();

    // Draw the bottom graph mirrored in the bottom half of the viewport
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height / 2);
    m_bottomGraph.draw();
}

} // namespace rg
