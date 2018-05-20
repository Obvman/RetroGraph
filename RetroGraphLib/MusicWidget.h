#pragma once

#include "stdafx.h"

#include <memory>

#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class MusicMeasure;

class MusicWidget : public Widget {
public:
    MusicWidget(const FontManager* fontManager,
                const std::unique_ptr<MusicMeasure>& musicMeasure,
                bool visible) :
        Widget{ fontManager, visible }, m_musicMeasure{ musicMeasure.get() } {}

    ~MusicWidget() noexcept = default;
    MusicWidget(const MusicWidget&) = delete;
    MusicWidget& operator=(const MusicWidget&) = delete;
    MusicWidget(MusicWidget&&) = delete;
    MusicWidget& operator=(MusicWidget&&) = delete;

    void updateObservers(const RetroGraph& rg) override;
    void draw() const override;

private:
    const MusicMeasure* m_musicMeasure;
};

} // namespace rg
