module RG.Widgets:WidgetContainer;

import RG.Rendering;

import "GLHeaderUnit.h";
import "RGAssert.h";

namespace rg {

WidgetContainer::WidgetContainer(WidgetPosition p, bool drawBackground)
    : m_children{}
    , m_viewport{}
    , m_pos{ p }
    , m_type{ getFillTypeFromPosition(p) }
    , m_drawBackground{ drawBackground } {

}

WidgetContainer::~WidgetContainer() {
    for (auto* widget : m_children) {
        widget->clear();
    }
}

bool WidgetContainer::isVisible() const {
    return !m_children.empty();
}

void rg::WidgetContainer::setViewport(int windowWidth, int windowHeight, WidgetPosition pos) {
    const auto widgetW{ windowWidth / 5 };
    const auto widgetH{ windowHeight / 6 };
    const auto sideWidgetH{ windowHeight / 2 };

    m_pos = pos;

    Viewport vp{};
    switch (pos) {
        case WidgetPosition::BOT_LEFT:
            vp = { screenMarginX, screenMarginY, widgetW, widgetH };
            break;
        case WidgetPosition::BOT_MID:
            vp = { screenMarginX + windowWidth/2 - widgetW, screenMarginY, widgetW * 2, widgetH };
            break;
        case WidgetPosition::BOT_RIGHT:
            vp = { windowWidth - widgetW - screenMarginX, screenMarginY, widgetW, widgetH };
            break;
        case WidgetPosition::MID_LEFT:
            vp = { screenMarginX, windowHeight/2 - windowHeight/4, widgetW, sideWidgetH };
            break;
        case WidgetPosition::MID_MID:
            vp = { screenMarginX + windowWidth/2 - widgetW, windowHeight/2 - windowHeight/4, 2 * widgetW, sideWidgetH };
            break;
        case WidgetPosition::MID_RIGHT:
            vp = { windowWidth - widgetW - screenMarginX, windowHeight/2 - windowHeight/4, widgetW, sideWidgetH };
            break;
        case WidgetPosition::TOP_LEFT:
            vp = {  screenMarginX, windowHeight - screenMarginY - widgetH, widgetW, widgetH };
            break;
        case WidgetPosition::TOP_MID:
            vp = { screenMarginX + windowWidth/2 - widgetW, windowHeight - screenMarginY - widgetH, widgetW * 2, widgetH };
            break;
        case WidgetPosition::TOP_RIGHT:
            vp = { windowWidth - widgetW - screenMarginX, windowHeight - screenMarginY - widgetH, widgetW, widgetH }; 
            break;
        default:
            break;
    }
    setChildViewports(vp, pos);
    invalidate();
}

void WidgetContainer::addChild(Widget * child) {
    m_children.push_back(child);
    invalidate();
}

void WidgetContainer::removeChild(Widget * child) {
    m_children.erase(std::remove(m_children.begin(), m_children.end(), child), m_children.end());
    invalidate();
}

void WidgetContainer::draw() {
    if (isVisible()) {
        for (auto* widget : m_children) {
            if (widget->needsRedraw()) {
                widget->clear();

                setGLViewport(widget->getViewport());

                if (m_drawBackground)
                    rg::drawWidgetBackground();

                GLListContainer::inst().drawTopAndBottomSerifs();

                widget->draw();
                widget->validate();
            }
        }
    }
}

void WidgetContainer::invalidate() {
    for (auto* widget : m_children) {
        widget->invalidate();
    }
}

void WidgetContainer::setChildViewports(const Viewport & vp, WidgetPosition pos) {
    m_viewport = vp;

    const auto numWidgets{ static_cast<GLint>(m_children.size()) };
    for (auto i = int{ 0 }; i < numWidgets; ++i) {
        switch (m_type) {
            case ContainerType::VFill: {
                const auto height{ m_viewport.height / static_cast<GLint>(m_children.size()) };
                const auto startY{ m_viewport.y + (height * (numWidgets - 1 - i)) };
                m_children[i]->setViewport(Viewport{ m_viewport.x, startY, m_viewport.width, height });
                break;
            }
            case ContainerType::HFill: {
                const auto width{ m_viewport.width / static_cast<GLint>(m_children.size()) };
                const auto startX{ m_viewport.x + (width * i) };
                m_children[i]->setViewport(Viewport{ startX, m_viewport.y, width, m_viewport.height });
                break;
            }
            case ContainerType::Single:
                RGASSERT(numWidgets <= 1, "Attempting to place too many widgets in the same position");
                m_children[i]->setViewport(m_viewport);
                break;
            case ContainerType::Mini:
                RGASSERT(numWidgets <= 1, "Attempting to place too many widgets in the same position");
                m_children[i]->setViewport(getMiniViewport(pos));
                break;
            default:
                RGERROR("Invalid ContainerType specified");
                break;
        }
    }
}

Viewport WidgetContainer::getMiniViewport(WidgetPosition pos) const {
    const auto miniWidgetWidth{ m_viewport.width / 5 };
    const auto miniWidgetHeight{ m_viewport.height / 4 };

    switch (pos) {
        case WidgetPosition::BOT_LEFT:
            return { m_viewport.x,
                     m_viewport.y,
                     miniWidgetWidth,
                     miniWidgetHeight };
        case WidgetPosition::BOT_RIGHT:
            return { m_viewport.x + m_viewport.width - miniWidgetWidth,
                     m_viewport.y,
                     miniWidgetWidth,
                     miniWidgetHeight };
        case WidgetPosition::TOP_LEFT:
            return { m_viewport.x,
                     m_viewport.y + m_viewport.height - miniWidgetHeight,
                     miniWidgetWidth,
                     miniWidgetHeight };
        case WidgetPosition::TOP_RIGHT:
            return { m_viewport.x + m_viewport.width - miniWidgetWidth,
                     m_viewport.y + m_viewport.height - miniWidgetHeight,
                     miniWidgetWidth,
                     miniWidgetHeight };
        default:
            RGERROR("Invalid mini viewport position");
    }
    return Viewport{};
}

ContainerType WidgetContainer::getFillTypeFromPosition(WidgetPosition p) const {
    if (p == WidgetPosition::TOP_MID || p == WidgetPosition::BOT_MID)
        return ContainerType::HFill;
    else if (p == WidgetPosition::MID_LEFT || p == WidgetPosition::MID_RIGHT)
        return ContainerType::VFill;
    else 
        return ContainerType::Single;
}

} // namespace rg
