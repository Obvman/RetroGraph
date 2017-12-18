#pragma once

#include "drawUtils.h"

namespace rg {

class FontManager;

class MainWidget {
public:
    MainWidget(const FontManager* fontManager, bool visible) :
        m_fontManager{ fontManager }, m_visible{ visible } {}
    ~MainWidget() noexcept = default;
    MainWidget(const MainWidget&) = delete;
    MainWidget& operator=(const MainWidget&) = delete;

    void draw() const;
    void clear() const;

    void setViewport(Viewport vp) { m_viewport = vp; };

    void setVisibility(bool b);
private:

    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
    bool m_visible{ true };
};

}
