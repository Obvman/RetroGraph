#pragma once

#include "stdafx.h"

#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class FPSWidget : public Widget {
public:
    FPSWidget(const FontManager* fontManager) :
        Widget{ fontManager, true } { }
    ~FPSWidget() noexcept = default;
    FPSWidget(const FPSWidget&) = delete;
    FPSWidget& operator=(const FPSWidget&) = delete;
    FPSWidget(FPSWidget&&) = delete;
    FPSWidget& operator=(FPSWidget&&) = delete;

    void draw() const override;

};

} // namespace rg