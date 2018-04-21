#pragma once

#include <memory>
#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class AnimationState;

class MainWidget : public Widget {
public:
    MainWidget(const FontManager* fontManager, 
               const std::unique_ptr<AnimationState>& as, bool visible) :
        Widget{ fontManager, visible }, m_animationState{ as } { }
    ~MainWidget() noexcept = default;
    MainWidget(const MainWidget&) = delete;
    MainWidget& operator=(const MainWidget&) = delete;
    MainWidget(MainWidget&&) = delete;
    MainWidget& operator=(MainWidget&&) = delete;

    /* Checks if the widget should draw to maintain the target FPS */
    bool needsDraw(uint32_t ticks) const;
    void draw() const override;
private:
    const std::unique_ptr<AnimationState>& m_animationState;
};

} // namespace rg
