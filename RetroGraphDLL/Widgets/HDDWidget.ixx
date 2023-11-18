export module Widgets.HDDWidget;

import IRetroGraph; // Reverse Dependency

import Measures.DriveMeasure;

import Rendering.FontManager;

import Widgets.Widget;

namespace rg {

export class HDDWidget : public Widget {
public:
    HDDWidget(const FontManager* fontManager,
              const IRetroGraph& rg, bool visible) :
        Widget{ fontManager, visible },
        m_driveMeasure{ &rg.getDriveMeasure() } { }

    ~HDDWidget() noexcept = default;
    HDDWidget(const HDDWidget&) = delete;
    HDDWidget& operator=(const HDDWidget&) = delete;
    HDDWidget(HDDWidget&&) = delete;
    HDDWidget& operator=(HDDWidget&&) = delete;

    void updateObservers(const IRetroGraph& rg) override;
    void draw() const override;
private:
    const DriveMeasure* m_driveMeasure;
};

} // namespace rg
