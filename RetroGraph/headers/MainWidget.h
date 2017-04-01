#pragma once

#include "Widget.h"
#include "drawUtils.h"

namespace rg {

class FontManager;

class MainWidget : public Widget {
public:
    MainWidget(const FontManager* fontManager) :
        m_fontManager{ fontManager } {}
    virtual ~MainWidget() noexcept = default;
    MainWidget(const MainWidget&) = delete;
    MainWidget& operator=(const MainWidget&) = delete;

    void draw() const override;

    void setViewport(Viewport vp) { m_viewport = vp; };
private:

    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
};

}
