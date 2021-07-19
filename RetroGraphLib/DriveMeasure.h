#pragma once

#include "stdafx.h"

#include <vector>
#include <string>
#include <memory>

#include "units.h"
#include "Measure.h"

namespace rg {

constexpr auto maxVolumeNameSize = int{ 64U };

/* Per drive data storage container */
class DriveInfo {
public:
    DriveInfo(char _driveLetter, uint64_t initFreeBytes, uint64_t totalBytes, const char* volumeName);
    ~DriveInfo() noexcept = default;
    DriveInfo(const DriveInfo&) = default;
    DriveInfo& operator=(const DriveInfo&) = default;
    DriveInfo(DriveInfo&&) = default;
    DriveInfo& operator=(DriveInfo&&) = default;

    void updateCapacityStr();

    char driveLetter{ 'A' };
    uint64_t totalFreeBytes{ 0U };
    uint64_t totalBytes{ 0U };
    std::string volumeName{ "" };
    std::string capacityStr{ "" };
};

/* Stores paths and statistics about all the system's fixed drives */
class DriveMeasure : public Measure {
public:
    DriveMeasure();
    ~DriveMeasure() noexcept = default;

    /* Updates each drive with new values */
    void update(int ticks) override;

    /* Returns the number of fixed drives active in the system */
    size_t getNumDrives() const { return m_drives.size(); }

    /* Returns the drive list */
    const std::vector<DriveInfo>& getDrives() const { return m_drives; }

    void refreshSettings() override { }
private:
    bool shouldUpdate(int ticks) const override;

    std::vector<std::string> m_drivePaths{ };
    std::vector<DriveInfo> m_drives{ };
};

} // namespace rg

