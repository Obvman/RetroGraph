export module RG.Widgets.Graph:GraphPointBuffer;

import RG.Rendering;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

// A rolling buffer for graph points. It takes up twice the space in memory as a naive buffer but has
// the advantage of only requiring points to be copied/replaced in memory once every numPoints() updates.
// Points are stored in the GL viewport space.
export class GraphPointBuffer {
public:
    explicit GraphPointBuffer(size_t numPoints_);

    void setPoints(std::span<const float> values);

    // Returns true if the underlying buffer was moved.
    // Returns false if the point was pushed in place
    bool pushPoint(float value);

    glm::vec2& operator[](size_t index) { return m_rollingBuffer[tail() + index]; }
    const glm::vec2& operator[](size_t index) const { return m_rollingBuffer[tail() + index]; }

    const glm::vec2* data() const { return m_rollingBuffer.data(); }
    const glm::vec2* back() const { return &m_rollingBuffer[m_head]; }
    const glm::vec2* front() const { return &m_rollingBuffer[m_tail]; }
    GLsizei head() const { return static_cast<GLsizei>(m_head); }
    GLsizei tail() const { return static_cast<GLsizei>(m_tail); }

    int bufferSize() const { return static_cast<GLsizei>(m_rollingBuffer.size()); }
    int numPoints() const { return bufferSize() / 2; }
    float getHorizontalPointInterval() const { return viewportWidth / (numPoints() - 1); }

private:
    void initPoints();

    std::vector<glm::vec2> m_rollingBuffer;
    size_t m_head;
    size_t m_tail;
};

}
