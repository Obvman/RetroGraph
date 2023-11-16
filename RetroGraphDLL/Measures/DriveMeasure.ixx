module;

#include "RGAssert.h"

export module Measures.DriveMeasure;

import Colors;
import Units;
import Utils;
import Measures.Measure;
import Measures.Data.DriveInfo;

import std.core;

import "WindowsHeaders.h";

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

    void refreshSettings() override { }
private:
    bool shouldUpdate(int ticks) const override;

    std::vector<std::string> m_drivePaths{ };
    std::vector<DriveInfo> m_drives{ };
};


DriveInfo::DriveInfo(char _driveLetter, uint64_t initFreeBytes, uint64_t totalBytes, const char* volumeName)
    : driveLetter{ _driveLetter }
    , totalFreeBytes{ initFreeBytes }
    , totalBytes{ totalBytes }
    , volumeName{ volumeName }
    , capacityStr{ "" } {

    updateCapacityStr();
}

void DriveInfo::updateCapacityStr() {
    const auto capacity{ bToGB(totalBytes) };
    if (capacity < 1000) {
        capacityStr = std::format("{}GB", capacity);
    } else {
        capacityStr = std::format("{:.1f}TB", capacity / 1024.0f);
    }
}

DriveMeasure::DriveMeasure() { 
    // Enumerate all available logical drives and store the drive paths
    const auto driveMask{ GetLogicalDrives() };
    for (int8_t i{ 0U }; i < 26; ++i) {
        if ((driveMask & (1 << i))) {
            char driveName[] = { 'A' + i, ':', '\\', '\0' };
            m_drivePaths.emplace_back(driveName);
        }
    }

    // For each drive path get the disk free/total bytes
    for (const auto& drivePath : m_drivePaths) {
        ULARGE_INTEGER freeBytesAvailable;
        ULARGE_INTEGER totalBytes;
        ULARGE_INTEGER totalFreeBytes;
        GetDiskFreeSpaceEx(drivePath.c_str(), &freeBytesAvailable, &totalBytes,
                           &totalFreeBytes);

        // Get the volume name
        char volumeNameBuff[maxVolumeNameSize];
        GetVolumeInformation(drivePath.c_str(), volumeNameBuff,
                             maxVolumeNameSize, nullptr, nullptr, nullptr,
                             nullptr, 0);

        // Only add if the drive is a fixed HDD/SSD
        const auto dType{ GetDriveType(drivePath.c_str()) };
        if (dType == DRIVE_FIXED) {
            m_drives.emplace_back(
                drivePath[0],
                totalFreeBytes.QuadPart,
                totalBytes.QuadPart,
                volumeNameBuff
            );
        }
    }
}

void DriveMeasure::update(int ticks) {
    if (ticksMatchSeconds(ticks, 15)) {
        /* Refresh drive statistics. We won't consider drives being
         * added/removed since these are fixed drives and the program shouldn't
         * be running in those events
         */
        for (auto& di : m_drives) {
            char path[] = { di.driveLetter, ':', '\\', '\0' };

            ULARGE_INTEGER freeBytesAvailable;
            ULARGE_INTEGER totalBytes;
            ULARGE_INTEGER totalFreeBytes;
            GetDiskFreeSpaceEx(path, &freeBytesAvailable,
                               &totalBytes, &totalFreeBytes);

            di.totalFreeBytes = totalFreeBytes.QuadPart;

            // Some rare cases allow a drive's max capacity to change
            // (like unlocking a bitlocked drive)
            if (totalBytes.QuadPart != di.totalBytes) {
                di.totalBytes = totalBytes.QuadPart;
                di.updateCapacityStr();
            }
        }
    }

    // Check for drive name updates every 20 minutes
    if (ticksMatchSeconds(ticks, 60 * 20)) {
        for (auto& di : m_drives) {
            char path[] = { di.driveLetter, ':', '\\', '\0' };
            char volumeNameBuff[maxVolumeNameSize];
            GetVolumeInformation(path, volumeNameBuff, maxVolumeNameSize,
                                 nullptr, nullptr, nullptr, nullptr, 0);

            if (di.volumeName != volumeNameBuff) {
                di.volumeName = std::string{ volumeNameBuff };
            }
        }
    }
}

bool DriveMeasure::shouldUpdate(int ticks) const {
    return ticksMatchSeconds(ticks, 15) || ticksMatchSeconds(ticks, 60 * 20);
}

}
