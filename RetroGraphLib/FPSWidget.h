#pragma once

#include "stdafx.h"

#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class FPSWidget : public Widget {
public:
    FPSWidget(const FontManager* fontManager, bool visible) :
        Widget{ fontManager, visible } { }
    ~FPSWidget() noexcept = default;
    FPSWidget(const FPSWidget&) = delete;
    FPSWidget& operator=(const FPSWidget&) = delete;
    FPSWidget(FPSWidget&&) = delete;
    FPSWidget& operator=(FPSWidget&&) = delete;

    void updateObservers(const RetroGraph&) override { /* Empty */ }
    void draw() const override;

};

} // namespace rg