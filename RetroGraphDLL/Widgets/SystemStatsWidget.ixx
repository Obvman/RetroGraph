export module Widgets.SystemStatsWidget;

import IRetroGraph; // Reverse Dependency

import Rendering.FontManager;

import Widgets.Widget;

namespace rg {

export class SystemStatsWidget : public Widget {
public:
    SystemStatsWidget(const FontManager* fontManager,
                      const IRetroGraph& rg, bool visible);
    ~SystemStatsWidget() noexcept = default;

    void draw() const override;
private:
    std::vector<std::string> m_statsStrings{ };
};

} // namespace rg
