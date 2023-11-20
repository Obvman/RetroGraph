export module Widgets.MusicWidget;

import Measures.MusicMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.memory;

namespace rg {

export class MusicWidget : public Widget {
public:
    MusicWidget(const FontManager* fontManager,
                std::shared_ptr<MusicMeasure const> musicMeasure, bool visible) :
        Widget{ fontManager, visible },
        m_musicMeasure{ musicMeasure } {}

    ~MusicWidget() noexcept = default;
    MusicWidget(const MusicWidget&) = delete;
    MusicWidget& operator=(const MusicWidget&) = delete;
    MusicWidget(MusicWidget&&) = delete;
    MusicWidget& operator=(MusicWidget&&) = delete;

    void draw() const override;

private:
    std::shared_ptr<MusicMeasure const> m_musicMeasure;
};

} // namespace rg
