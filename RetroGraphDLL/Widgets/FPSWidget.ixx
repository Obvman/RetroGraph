export module Widgets.FPSWidget;

import IRetroGraph; // Reverse Dependency

import Rendering.FontManager;

import Widgets.Widget;

namespace rg {

export class FPSWidget : public Widget {
public:
    FPSWidget(const FontManager* fontManager, bool visible) :
        Widget{ fontManager, visible } { }
    ~FPSWidget() noexcept = default;
    FPSWidget(const FPSWidget&) = delete;
    FPSWidget& operator=(const FPSWidget&) = delete;
    FPSWidget(FPSWidget&&) = delete;
    FPSWidget& operator=(FPSWidget&&) = delete;

    void updateObservers(const IRetroGraph&) override { /* Empty */ }
    void draw() const override;
};

}
