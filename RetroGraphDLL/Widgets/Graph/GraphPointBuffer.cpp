module Widgets.Graph.GraphPointBuffer;

namespace rg {

GraphPointBuffer::GraphPointBuffer(size_t numPoints_)
    : m_rollingBuffer{ numPoints_ * 2 }
    , m_head{ numPoints_ == 0 ? 0 : numPoints_ - 1 }
    , m_tail{ 0 } {

    // Set initial points
    if (numPoints() != 0) {
        initPoints();
    }
}

void GraphPointBuffer::setPoints(std::span<const float> values) {
    m_head = values.empty() ? 0 : values.size() - 1;
    m_tail = 0;
    m_rollingBuffer.resize(values.size() * 2);

    if (values.size() == 1) {
        m_rollingBuffer[0] = { 0.0f, values[0] };
        m_rollingBuffer[1] = { 0.0f + 2*viewportWidth, viewportMin };
    } else if (!values.empty()) {
        for (size_t i = m_tail; i <= m_head; ++i) {
            const auto x{ static_cast<float>(i) / m_head * viewportWidth };
            // Copy the values into the first half of the buffer
            m_rollingBuffer[i] = { x, values[i] };

            // Initialize the static x values in the second half of the buffer.
            // y values will be written as the buffer rolls forward
            m_rollingBuffer[i + m_head + 1] = { x + viewportWidth + getHorizontalPointInterval(), viewportMin };
        }
    }
}

bool GraphPointBuffer::pushPoint(float value) {
    ++m_head;
    ++m_tail;
    m_rollingBuffer[m_head].y = value;

    // When the head of the rolling buffer hits the end, copy the second half of the buffer into the first half
    // and update the pointers to target the first half of the buffer.
    if (m_head == bufferSize() - 1) {
        for (size_t i = m_tail; i <= m_head; ++i) {
            m_rollingBuffer[i - m_tail].y = m_rollingBuffer[i].y;
        }
        m_head -= m_tail;
        m_tail = 0;

        // All of the points have changed in this case so update the whole range of points in the VBO
        return true;
    } else {
        // Only one point has changed here
        return false;
    }
}

void GraphPointBuffer::initPoints() {
    for (size_t i = m_tail; i <= m_head; ++i) {
        const auto x{ static_cast<float>(i) / m_head * viewportWidth };
        // Copy the values into the first half of the buffer
        m_rollingBuffer[i] = { x, viewportMin };

        // Initialize the static x values in the second half of the buffer.
        // y values will be written as the buffer rolls forward
        m_rollingBuffer[i + m_head + 1] = { x + viewportWidth + getHorizontalPointInterval(), viewportMin };
    }
}

}
