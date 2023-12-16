export module Widgets.HDDWidget;

import RG.Measures;
import RG.Rendering;

import Widgets.Widget;

import std.core;

namespace rg {

export class HDDWidget : public Widget {
public:
    HDDWidget(const FontManager* fontManager, std::shared_ptr<const DriveMeasure> driveMeasure);
    ~HDDWidget() noexcept;

    void draw() const override;

private:
    PostUpdateEvent::Handle RegisterPostUpdateCallback();

    std::shared_ptr<const DriveMeasure> m_driveMeasure;
    PostUpdateEvent::Handle m_postUpdateHandle;
};

} // namespace rg
