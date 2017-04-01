#pragma once

#include "Widget.h"
#include "drawUtils.h"

namespace rg {

class FontManager;

class MainWidget : public Widget {
public:
    MainWidget() = default;
    virtual ~MainWidget() = default;
    MainWidget(const MainWidget&) = delete;
    MainWidget& operator=(const MainWidget&) = delete;

    virtual void draw() const override;

    void init(const FontManager* fontManager);

    void setViewport(Viewport vp) { m_viewport = vp; };
private:

    const FontManager* m_fontManager;
    Viewport m_viewport;
};

}
