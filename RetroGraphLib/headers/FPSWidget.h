#pragma once

#include "../headers/drawUtils.h"

namespace rg {

class FontManager;

class FPSWidget {
public:
    FPSWidget(const FontManager* fm) :
        m_fontManager{ fm } { /* Empty */ }
    ~FPSWidget() noexcept = default;
    FPSWidget(const FPSWidget&) = delete;
    FPSWidget& operator=(const FPSWidget&) = delete;
    FPSWidget(FPSWidget&&) = delete;
    FPSWidget& operator=(FPSWidget&&) = delete;

    void draw() const;

    void setViewport(Viewport vp) { m_viewport = vp; };

    void clear() const;

    void setVisibility(bool v);

private:
    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
    bool m_visible{ true };

};

} // namespace rg