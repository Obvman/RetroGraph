#pragma once

#include "drawUtils.h"

namespace rg {

class FontManager;
class MusicMeasure;

class MusicWidget {
public:
    MusicWidget(const FontManager* fontManager,
                const MusicMeasure* musicMeasure,
                bool visible) :
        m_fontManager{ fontManager }, m_visible{ visible }, 
        m_musicMeasure{ musicMeasure } {}

    ~MusicWidget() noexcept = default;
    MusicWidget(const MusicWidget&) = delete;
    MusicWidget& operator=(const MusicWidget&) = delete;

    void draw() const;

    void setViewport(Viewport vp) { m_viewport = vp; };
private:
    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
    bool m_visible{ true };

    const MusicMeasure* m_musicMeasure{ nullptr };
};

}
