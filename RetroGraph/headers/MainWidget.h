#pragma once

#include "drawUtils.h"

namespace rg {

class FontManager;

class MainWidget {
public:
    MainWidget(const FontManager* fontManager) :
        m_fontManager{ fontManager } {}
    ~MainWidget() noexcept = default;
    MainWidget(const MainWidget&) = delete;
    MainWidget& operator=(const MainWidget&) = delete;

    void draw() const;

    void setViewport(Viewport vp) { m_viewport = vp; };
private:

    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
};

}
