#pragma once

#include <memory>

#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class FontManager;
class DriveMeasure;

class HDDWidget : public Widget {
public:
    HDDWidget(const FontManager* fontManager, 
              const std::unique_ptr<DriveMeasure>& driveMeasure, bool visible) :
        Widget{ visible }, m_fontManager{ fontManager },
        m_driveMeasure{ driveMeasure } { /* Empty */ }

    ~HDDWidget() noexcept = default;
    HDDWidget(const HDDWidget&) = delete;
    HDDWidget& operator=(const HDDWidget&) = delete;
    HDDWidget(HDDWidget&&) = delete;
    HDDWidget& operator=(HDDWidget&&) = delete;

    void draw() const override;
private:
    const FontManager* m_fontManager{ nullptr };

    const std::unique_ptr<DriveMeasure>& m_driveMeasure;
};

} // namespace rg
