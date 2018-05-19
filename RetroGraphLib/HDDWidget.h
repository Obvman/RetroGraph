#pragma once

#include "stdafx.h"

#include <memory>

#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class DriveMeasure;

class HDDWidget : public Widget {
public:
    HDDWidget(const FontManager* fontManager, 
              const std::unique_ptr<DriveMeasure>& driveMeasure, bool visible) :
        Widget{ fontManager, visible }, m_driveMeasure{ driveMeasure } { }

    ~HDDWidget() noexcept = default;
    HDDWidget(const HDDWidget&) = delete;
    HDDWidget& operator=(const HDDWidget&) = delete;
    HDDWidget(HDDWidget&&) = delete;
    HDDWidget& operator=(HDDWidget&&) = delete;

    void draw() const override;
private:
    const std::unique_ptr<DriveMeasure>& m_driveMeasure;
};

} // namespace rg
