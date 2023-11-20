module Widgets.Widget;

import Rendering.DrawUtils;

namespace rg {

void Widget::clear() const {
    viewport(m_viewport);
    scissorClear(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
}

void Widget::setVisibility(bool b) {
    m_visible = b;
    if (m_visible) {
        draw();
    } else {
        clear();
    }
}

} // namespace rg
