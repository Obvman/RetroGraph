export module Widgets.MusicWidget;

import RG.Measures;
import RG.Rendering;

import Widgets.Widget;

import std.memory;

namespace rg {

export class MusicWidget : public Widget {
public:
    MusicWidget(const FontManager* fontManager, std::shared_ptr<const MusicMeasure> musicMeasure);
    ~MusicWidget() noexcept;

    void draw() const override;

private:
    PostUpdateEvent::Handle RegisterPostUpdateCallback();

    std::shared_ptr<const MusicMeasure> m_musicMeasure;
    PostUpdateEvent::Handle m_postUpdateHandle;
};

} // namespace rg
