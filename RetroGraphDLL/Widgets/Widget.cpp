module RG.Widgets:Widget;

import RG.Rendering;

namespace rg {

Widget::~Widget() {
    clear();
}

void Widget::clear() const {
    if (!m_viewport.isEmpty()) {
        setGLViewport(m_viewport);
        scissorClear(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
    }
}

} // namespace rg
