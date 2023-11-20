export module Widgets.FPSWidget;

import Rendering.FontManager;

import Widgets.Widget;

namespace rg {

export class FPSWidget : public Widget {
public:
    FPSWidget(const FontManager* fontManager) :
        Widget{ fontManager } { }
    ~FPSWidget() noexcept = default;

    void draw() const override;
};

}
