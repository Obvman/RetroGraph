export module Measures.DisplayMeasure;

import Monitors;
import Measures.Measure;

namespace rg {

export class DisplayMeasure : public Measure {
public:
    DisplayMeasure();
    ~DisplayMeasure() noexcept = default;

    void update(int /*ticks*/) override { /* Do nothing */ }

    const Monitors* getMonitors() const { return &m_monitors; }

    void refreshSettings() override { }

private:
    Monitors m_monitors;
};

} // namespace rg
