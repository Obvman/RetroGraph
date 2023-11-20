export module Widgets.HDDWidget;

import Measures.DriveMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.core;

namespace rg {

export class HDDWidget : public Widget {
public:
    HDDWidget(const FontManager* fontManager, std::shared_ptr<const DriveMeasure> driveMeasure) :
        Widget{ fontManager },
        m_driveMeasure{ driveMeasure } { }

    ~HDDWidget() noexcept = default;

    void draw() const override;
private:
    std::shared_ptr<const DriveMeasure> m_driveMeasure;
};

} // namespace rg
