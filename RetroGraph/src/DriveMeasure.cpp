#include "../headers/DriveMeasure.h"

#include <Windows.h>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <GL/glew.h>
#include <GL/gl.h>
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/freeglut.h>

#include "../headers/colors.h"
#include "../headers/utils.h"

namespace rg {

DriveMeasure::DriveMeasure() :
    m_drivePaths{},
    m_drives{} {
}

DriveMeasure::~DriveMeasure() {
}

void DriveMeasure::init() {
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
        GetDiskFreeSpaceEx(drivePath.c_str(), &freeBytesAvailable, &totalBytes, &totalFreeBytes);

        // Get the volume name
        char volumeNameBuff[maxVolumeNameSize];
        GetVolumeInformation(drivePath.c_str(), volumeNameBuff, maxVolumeNameSize,
                             nullptr, nullptr, nullptr, nullptr, 0);

        // Check the drive is a fixed HDD/SSD
        const auto dType{ GetDriveType(drivePath.c_str()) };
        if (dType == DRIVE_FIXED) {
            m_drives.emplace_back(std::make_unique<DriveInfo>(
                drivePath[0], totalFreeBytes.QuadPart, totalBytes.QuadPart, volumeNameBuff
            ));
        }
    }
}

void DriveMeasure::update(uint32_t ticks) {
    if ((ticks % (ticksPerSecond * 2)) == 0) {
        /* Refresh drive statistics. We won't consider drives being added/removed
           since these are fixed drives and the program shouldn't be running in
           those events */
        for (const auto& pdi : m_drives) {
            char path[4] = { pdi->driveLetter, ':', '\\', '\0' };

            // Only update drive capacity every 15 seconds
            if ((ticks % (ticksPerSecond * 15)) == 0) {

                ULARGE_INTEGER freeBytesAvailable;
                ULARGE_INTEGER totalBytes;
                ULARGE_INTEGER totalFreeBytes;
                GetDiskFreeSpaceEx(path, &freeBytesAvailable,
                                   &totalBytes, &totalFreeBytes);

                // We don't expect the max capacity of a fixed drive to change,
                // so only update the DriveInfo with the freeBytes
                pdi->totalFreeBytes = totalFreeBytes.QuadPart;
            }

            // Only check for drive name updates every 20 minutes
            if ((ticks % (ticksPerSecond * 60 * 20)) == 0) {
                char volumeNameBuff[maxVolumeNameSize];
                GetVolumeInformation(path, volumeNameBuff, maxVolumeNameSize,
                                     nullptr, nullptr, nullptr, nullptr, 0);

                // If the volume name has been changed, update the drive info object
                if (pdi->volumeName != volumeNameBuff) {
                    pdi->volumeName = std::string{ volumeNameBuff };
                }
            }
        }
    }
}

}
