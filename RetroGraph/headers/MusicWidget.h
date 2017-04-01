#pragma once

#include "Widget.h"
#include "drawUtils.h"

namespace rg {

class FontManager;
class MusicMeasure;

class MusicWidget : public Widget {
public:
    MusicWidget(const FontManager* fontManager, const MusicMeasure* musicMeasure) :
        m_fontManager{ fontManager }, m_musicMeasure{ musicMeasure } {}

    virtual ~MusicWidget() noexcept = default;
    MusicWidget(const MusicWidget&) = delete;
    MusicWidget& operator=(const MusicWidget&) = delete;

    void draw() const override;

    void setViewport(Viewport vp) { m_viewport = vp; };
private:
    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };

    const MusicMeasure* m_musicMeasure{ nullptr };
};

}
