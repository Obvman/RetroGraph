#pragma once

#include <memory>
#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class FontManager;
class AnimationState;

class MainWidget : public Widget {
public:
    MainWidget(const FontManager* fontManager, 
               const std::unique_ptr<AnimationState>& as, bool visible) :
        Widget{ visible }, m_fontManager{ fontManager },
        m_animationState{ as } { /* Empty */ }
    ~MainWidget() noexcept = default;
    MainWidget(const MainWidget&) = delete;
    MainWidget& operator=(const MainWidget&) = delete;
    MainWidget(MainWidget&&) = delete;
    MainWidget& operator=(MainWidget&&) = delete;

    /* Checks if the widget should draw to maintain the target FPS */
    bool needsDraw(uint32_t ticks) const;
    void draw() const override;
private:
    const FontManager* m_fontManager{ nullptr };
    const std::unique_ptr<AnimationState>& m_animationState;
};

} // namespace rg
