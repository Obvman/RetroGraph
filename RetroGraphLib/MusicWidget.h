#pragma once

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
        Widget{ fontManager, visible }, m_musicMeasure{ musicMeasure } {}

    ~MusicWidget() noexcept = default;
    MusicWidget(const MusicWidget&) = delete;
    MusicWidget& operator=(const MusicWidget&) = delete;
    MusicWidget(MusicWidget&&) = delete;
    MusicWidget& operator=(MusicWidget&&) = delete;

    void draw() const override;

private:
    const std::unique_ptr<MusicMeasure>& m_musicMeasure;
};

} // namespace rg
