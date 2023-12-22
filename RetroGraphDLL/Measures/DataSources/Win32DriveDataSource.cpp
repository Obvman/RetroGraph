module RG.Measures.DataSources:Win32DriveDataSource;

import "WindowsHeaderUnit.h";

namespace rg {

constexpr auto maxVolumeNameSize = int{ 64U };

DriveData Win32DriveDataSource::getDriveData() const {
    DriveData driveData;

    std::vector<std::string> drivePaths;

    // Enumerate all available logical drives and store the drive paths
    const auto driveMask{ GetLogicalDrives() };
    for (int8_t i{ 0U }; i < 26; ++i) {
        if ((driveMask & (1 << i))) {
            char driveName[] = { 'A' + i, ':', '\\', '\0' };
            drivePaths.emplace_back(driveName);
        }
    }

    for (const auto& drivePath : drivePaths) {
        ULARGE_INTEGER freeBytesAvailable;
        ULARGE_INTEGER totalBytes;
        ULARGE_INTEGER totalFreeBytes;
        GetDiskFreeSpaceEx(drivePath.c_str(), &freeBytesAvailable,
                           &totalBytes, &totalFreeBytes);

        char volumeNameBuff[maxVolumeNameSize];
        GetVolumeInformation(drivePath.c_str(), volumeNameBuff, maxVolumeNameSize,
                             nullptr, nullptr, nullptr, nullptr, 0);

        driveData.drives.emplace_back(
            drivePath[0],
            totalFreeBytes.QuadPart,
            totalBytes.QuadPart,
            volumeNameBuff
        );
    }

    return driveData;
}

}
