export module Widgets.MusicWidget;

import Measures.MusicMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.memory;

namespace rg {

export class MusicWidget : public Widget {
public:
    MusicWidget(const FontManager* fontManager,
                std::shared_ptr<const MusicMeasure> musicMeasure) :
        Widget{ fontManager },
        m_musicMeasure{ musicMeasure } {}

    ~MusicWidget() noexcept = default;

    void draw() const override;

private:
    std::shared_ptr<const MusicMeasure> m_musicMeasure;
};

} // namespace rg
