#pragma once

#include "Widget.h"
#include "drawUtils.h"

namespace rg {

class FontManager;
class DriveMeasure;

class HDDWidget : public Widget {
public:
    HDDWidget() = default;
    virtual ~HDDWidget() noexcept = default;
    HDDWidget(const HDDWidget&) = delete;
    HDDWidget& operator=(const HDDWidget&) = delete;

    void draw() const override;

    void init(const FontManager* fontManager, const DriveMeasure* driveMeasure);

    void setViewport(Viewport vp) { m_viewport = vp; };

private:
    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };

    const DriveMeasure* m_driveMeasure{ nullptr };
};

}