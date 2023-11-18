export module Widgets.MusicWidget;

import IRetroGraph; // Reverse Dependency

import Measures.MusicMeasure;

import Rendering.FontManager;

import Widgets.Widget;

namespace rg {

export class MusicWidget : public Widget {
public:
    MusicWidget(const FontManager* fontManager,
                const IRetroGraph& rg, bool visible) :
        Widget{ fontManager, visible },
        m_musicMeasure{ &rg.getMusicMeasure() } {}

    ~MusicWidget() noexcept = default;
    MusicWidget(const MusicWidget&) = delete;
    MusicWidget& operator=(const MusicWidget&) = delete;
    MusicWidget(MusicWidget&&) = delete;
    MusicWidget& operator=(MusicWidget&&) = delete;

    void updateObservers(const IRetroGraph& rg) override;
    void draw() const override;

private:
    const MusicMeasure* m_musicMeasure;
};

} // namespace rg
