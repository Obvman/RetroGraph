#pragma once

#include "Widget.h"
#include "drawUtils.h"

namespace rg {

class FontManager;

class MainWidget : public Widget {
public:
    MainWidget();
    virtual ~MainWidget();
    MainWidget(const MainWidget&) = delete;
    MainWidget& operator=(const MainWidget&) = delete;

    virtual void draw() const override;

    void init(const FontManager* fontManager,
              Viewport viewport);

private:

    const FontManager* m_fontManager;
    Viewport m_viewport;
};

}
