#pragma once

#include <memory>

#include "drawUtils.h"

namespace rg {

class FontManager;
class DriveMeasure;

class HDDWidget {
public:
    HDDWidget(const FontManager* fontManager, 
              const std::unique_ptr<DriveMeasure>& driveMeasure, bool visible) :
        m_fontManager{ fontManager }, m_visible{ visible },
        m_driveMeasure{ driveMeasure } { /* Empty */ }

    ~HDDWidget() noexcept = default;
    HDDWidget(const HDDWidget&) = delete;
    HDDWidget& operator=(const HDDWidget&) = delete;

    void draw() const;
    void clear() const;

    void setViewport(Viewport vp) { m_viewport = vp; };
    void setVisibility(bool b);
private:
    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
    bool m_visible{ true };

    const std::unique_ptr<DriveMeasure>& m_driveMeasure{ nullptr };
};

}
