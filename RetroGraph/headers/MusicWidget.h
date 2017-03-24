#pragma once

#include "Widget.h"
#include "drawUtils.h"

namespace rg {

class FontManager;
class MusicMeasure;

class MusicWidget : public Widget {
public:
    MusicWidget();
    virtual ~MusicWidget();
    MusicWidget(const MusicWidget&) = delete;
    MusicWidget& operator=(const MusicWidget&) = delete;

    virtual void draw() const override;

    void init(const FontManager* fontManager, const MusicMeasure* musicMeasure,
              Viewport viewport);

private:
    void drawProgressBar() const;

    const FontManager* m_fontManager;
    Viewport m_viewport;

    const MusicMeasure* m_musicMeasure;
};

}
