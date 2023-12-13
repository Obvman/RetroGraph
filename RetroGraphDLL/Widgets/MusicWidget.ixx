export module Widgets.MusicWidget;

import Measures.MusicMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.memory;

namespace rg {

export class MusicWidget : public Widget {
public:
    MusicWidget(const FontManager* fontManager, std::shared_ptr<const MusicMeasure> musicMeasure);
    ~MusicWidget() noexcept;

    void draw() const override;

private:
    PostUpdateCallbackHandle RegisterPostUpdateCallback();

    std::shared_ptr<const MusicMeasure> m_musicMeasure;
    PostUpdateCallbackHandle m_postUpdateHandle;
};

} // namespace rg
