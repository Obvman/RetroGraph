export module DisplayMeasure;

import Measure;
import Monitors;

namespace rg {

export class DisplayMeasure;

class DisplayMeasure : public Measure {
public:
    DisplayMeasure();
    ~DisplayMeasure() noexcept = default;

    void update(int /*ticks*/) override { /* Do nothing */ }

    const Monitors* getMonitors() const { return &m_monitors; }

    void refreshSettings() override { }

private:
    Monitors m_monitors;
};

DisplayMeasure::DisplayMeasure()
    : m_monitors{} {

    force_update();
}

}
