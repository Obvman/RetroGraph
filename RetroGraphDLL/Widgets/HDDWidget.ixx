export module Widgets.HDDWidget;

import Measures.DriveMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.core;

namespace rg {

export class HDDWidget : public Widget {
public:
    HDDWidget(const FontManager* fontManager,
              std::shared_ptr<DriveMeasure const> driveMeasure, bool visible) :
        Widget{ fontManager, visible },
        m_driveMeasure{ driveMeasure } { }

    ~HDDWidget() noexcept = default;
    HDDWidget(const HDDWidget&) = delete;
    HDDWidget& operator=(const HDDWidget&) = delete;
    HDDWidget(HDDWidget&&) = delete;
    HDDWidget& operator=(HDDWidget&&) = delete;

    void draw() const override;
private:
    std::shared_ptr<DriveMeasure const> m_driveMeasure;
};

} // namespace rg
