export module Measures.DriveMeasure;

import Measures.Measure;
import Measures.Data.DriveInfo;

import std.core;

namespace rg {

constexpr auto maxVolumeNameSize = int{ 64U };

/* Stores paths and statistics about all the system's fixed drives */
export class DriveMeasure : public Measure {
public:
    DriveMeasure();
    ~DriveMeasure() noexcept = default;

    /* Updates each drive with new values */
    void update(int ticks) override;

    /* Returns the number of fixed drives active in the system */
    size_t getNumDrives() const { return m_drives.size(); }

    /* Returns the drive list */
    const std::vector<DriveInfo>& getDrives() const { return m_drives; }
private:
    bool shouldUpdate(int ticks) const override;

    std::vector<std::string> m_drivePaths{ };
    std::vector<DriveInfo> m_drives{ };
};

} // namespace rg
