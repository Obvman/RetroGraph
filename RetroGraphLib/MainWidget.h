#pragma once

#include "stdafx.h"

#include <memory>

#include "drawUtils.h"
#include "Widget.h"
#include "RetroGraph.h"

namespace rg {

class AnimationState;

class MainWidget : public Widget {
public:
    MainWidget(const FontManager* fontManager, 
               const RetroGraph& rg, bool visible) :
        Widget{ fontManager, visible }, m_animationState{ &rg.getAnimationState() } { }
    ~MainWidget() noexcept = default;
    MainWidget(const MainWidget&) = delete;
    MainWidget& operator=(const MainWidget&) = delete;
    MainWidget(MainWidget&&) = delete;
    MainWidget& operator=(MainWidget&&) = delete;

    /* Checks if the widget should draw to maintain the target FPS */
    bool needsDraw(uint32_t ticks) const;
    void updateObservers(const RetroGraph& rg) override;
    void draw() const override;
private:
    const AnimationState* m_animationState;
};

} // namespace rg
