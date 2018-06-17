#pragma once

#include "stdafx.h"

#include <vector>
#include <string>
#include <memory>

#include "units.h"
#include "Measure.h"

namespace rg {

constexpr auto maxVolumeNameSize = uint32_t{ 64U };

/* Per drive data storage container */
class DriveInfo {
public:
    DriveInfo(char _driveLetter, uint64_t initFreeBytes, uint64_t totalBytes,
              const char* volumeName) :
            driveLetter{ _driveLetter },
            totalFreeBytes{ initFreeBytes },
            totalBytes{ totalBytes },
            volumeName{ volumeName },
            capacityStr{ "" } {

        updateCapacityStr();
    }

    ~DriveInfo() noexcept = default;
    DriveInfo(const DriveInfo&) = default;
    DriveInfo& operator=(const DriveInfo&) = default;
    DriveInfo(DriveInfo&&) = default;
    DriveInfo& operator=(DriveInfo&&) = default;

    void updateCapacityStr() {
        const auto capacity{ bToGB(totalBytes) };
        if (capacity < 1000) {
            capacityStr = std::to_string(capacity) + "GB";
        } else {
            char buff[6];
            snprintf(buff, sizeof(buff), "%.1fTB", capacity / 1024.0f);
            capacityStr = buff;
        }
    }

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
    DriveMeasure(const DriveMeasure&) = delete;
    DriveMeasure& operator=(const DriveMeasure&) = delete;
    DriveMeasure(DriveMeasure&&) = delete;
    DriveMeasure& operator=(DriveMeasure&&) = delete;

    /* Updates each drive with new values */
    void update(uint32_t ticks) override;

    /* Returns the number of fixed drives active in the system */
    size_t getNumDrives() const { return m_drives.size(); }

    /* Returns the drive list */
    const std::vector<std::unique_ptr<DriveInfo>>& getDrives() const { return m_drives; }

private:
    bool shouldUpdate(uint32_t ticks) const override;

    std::vector<std::string> m_drivePaths{ };
    std::vector<std::unique_ptr<DriveInfo>> m_drives{ };
};

} // namespace rg

