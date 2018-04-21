#pragma once

#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class FontManager;

class FPSWidget : public Widget {
public:
    FPSWidget(const FontManager* fm) :
        m_fontManager{ fm } { /* Empty */ }
    ~FPSWidget() noexcept = default;
    FPSWidget(const FPSWidget&) = delete;
    FPSWidget& operator=(const FPSWidget&) = delete;
    FPSWidget(FPSWidget&&) = delete;
    FPSWidget& operator=(FPSWidget&&) = delete;

    void draw() const override;

private:
    const FontManager* m_fontManager{ nullptr };
};

} // namespace rg