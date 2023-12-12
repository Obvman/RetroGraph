module Measures.DriveMeasure;

import "WindowsHeaderUnit.h";

namespace rg {

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

void DriveMeasure::updateInternal() {
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

    for (auto& di : m_drives) {
        char path[] = { di.driveLetter, ':', '\\', '\0' };
        char volumeNameBuff[maxVolumeNameSize];
        GetVolumeInformation(path, volumeNameBuff, maxVolumeNameSize,
                             nullptr, nullptr, nullptr, nullptr, 0);

        if (di.volumeName != volumeNameBuff) {
            di.volumeName = std::string{ volumeNameBuff };
        }
    }
    postUpdate();
}

} // namespace rg
