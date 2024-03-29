export module RG.Measures:DisplayMeasure;

import :Measure;

import Monitors;

namespace rg {

export class DisplayMeasure : public Measure {
public:
    DisplayMeasure();
    ~DisplayMeasure() noexcept = default;

    const Monitors* getMonitors() const { return &m_monitors; }

protected:
    bool updateInternal() override { return false; }

private:
    Monitors m_monitors;
};

} // namespace rg
