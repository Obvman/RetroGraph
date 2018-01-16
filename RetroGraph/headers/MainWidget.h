#pragma once

#include <memory>
#include "drawUtils.h"

namespace rg {

class FontManager;
class AnimationState;

class MainWidget {
public:
    MainWidget(const FontManager* fontManager, const std::unique_ptr<AnimationState>& as, bool visible) :
        m_fontManager{ fontManager }, m_animationState{ as }, m_visible{ visible } { }
    ~MainWidget() noexcept = default;
    MainWidget(const MainWidget&) = delete;
    MainWidget& operator=(const MainWidget&) = delete;

    void draw() const;
    void clear() const;

    void setViewport(Viewport vp) { m_viewport = vp; };

    void setVisibility(bool b);
private:

    const FontManager* m_fontManager{ nullptr };
    const std::unique_ptr<AnimationState>& m_animationState{ nullptr };
    Viewport m_viewport{ };
    bool m_visible{ true };
};

}
