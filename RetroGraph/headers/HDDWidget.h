#pragma once

#include "drawUtils.h"

namespace rg {

class FontManager;
class DriveMeasure;

class HDDWidget {
public:
    HDDWidget(const FontManager* fontManager, const DriveMeasure* driveMeasure) :
        m_fontManager{ fontManager },
        m_driveMeasure{ driveMeasure } {}

    ~HDDWidget() noexcept = default;
    HDDWidget(const HDDWidget&) = delete;
    HDDWidget& operator=(const HDDWidget&) = delete;

    void draw() const;

    void setViewport(Viewport vp) { m_viewport = vp; };
private:
    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };

    const DriveMeasure* m_driveMeasure{ nullptr };
};

}
