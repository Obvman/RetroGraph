#pragma once

#include "stdafx.h"

#include <string>
#include <vector>

#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class SystemMeasure;
class GPUMeasure;

class SystemStatsWidget : public Widget {
public:
    SystemStatsWidget(const FontManager* fontManager,
                      const RetroGraph& rg, bool visible);
    ~SystemStatsWidget() noexcept = default;

    void draw() const override;
private:
    std::vector<std::string> m_statsStrings{ };
};

} // namespace rg
