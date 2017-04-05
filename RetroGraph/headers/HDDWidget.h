#pragma once

#include "drawUtils.h"

namespace rg {

class FontManager;
class DriveMeasure;

class HDDWidget {
public:
    HDDWidget(const FontManager* fontManager, const DriveMeasure* driveMeasure,
              bool visible) :
        m_fontManager{ fontManager }, m_visible{ visible },
        m_driveMeasure{ driveMeasure } {}

    ~HDDWidget() noexcept = default;
    HDDWidget(const HDDWidget&) = delete;
    HDDWidget& operator=(const HDDWidget&) = delete;

    void draw() const;

    void setViewport(Viewport vp) { m_viewport = vp; };

    void setVisible(bool b) { m_visible = b; }
private:
    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
    bool m_visible{ true };

    const DriveMeasure* m_driveMeasure{ nullptr };
};

}
