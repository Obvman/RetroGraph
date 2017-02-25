#include "DriveMeasure.h"

#include <Windows.h>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include "colors.h"
#include "utils.h"

namespace rg {

DriveMeasure::DriveMeasure(GLint vpX, GLint vpY, GLint vpW, GLint vpH) :
    m_drivePaths{},
    m_viewportStartX{ vpX },
    m_viewportStartY{ vpY },
    m_viewportWidth{ vpW },
    m_viewportHeight{ vpH } {

    // Enumerate all available logical drives and store the drive paths
    auto driveMask = GetLogicalDrives();
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

DriveMeasure::~DriveMeasure() {
}

void DriveMeasure::init() {
    update(ticksPerSecond * 15);
}

void DriveMeasure::update(uint32_t ticks) {
    /* Refresh drive statistics. We won't consider drives being added/removed
       since these are fixed drives and the program shouldn't be running in
       those events */
    for (const auto& pdi : m_drives) {
        char path[4] = { pdi->getDriveLetter(), ':', '\\', '\0'};

        // Only update drive capacity every 15 seconds
        if ((ticks % (ticksPerSecond * 15)) == 0) {

            ULARGE_INTEGER freeBytesAvailable;
            ULARGE_INTEGER totalBytes;
            ULARGE_INTEGER totalFreeBytes;
            GetDiskFreeSpaceEx(path, &freeBytesAvailable,
                               &totalBytes, &totalFreeBytes);

            pdi->setTotalBytes(totalBytes.QuadPart);
            pdi->setTotalFreeBytes(totalFreeBytes.QuadPart);

            std::stringstream ss;
            ss << std::fixed << std::setprecision(2)
               << pdi->getDriveLetter() << ": "
               << pdi->getVolumeName() << " ("
               << static_cast<float>(pdi->getFreeBytes()) / GB << "GB/"
               << static_cast<float>(pdi->getTotalBytes()) / GB << "GB)\n";
            pdi->setDriveInfoStr(ss.str());
        }

        // Only check for drive name updates every 20 minutes
        if ((ticks % (ticksPerSecond * 20)) == 0) {
            char volumeNameBuff[maxVolumeNameSize];
            GetVolumeInformation(path, volumeNameBuff, maxVolumeNameSize,
                                 nullptr, nullptr, nullptr, nullptr, 0);

            // If the volume name has been changed, update the drive info object
            if (pdi->getVolumeName() != volumeNameBuff) {
                pdi->setVolumeName(std::string{volumeNameBuff});
            }
        }
    }
}

void DriveMeasure::draw() const {

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    glViewport(m_viewportStartX, m_viewportStartY, m_viewportWidth, m_viewportHeight);

    drawText();

    glViewport(vp[0], vp[1], vp[2], vp[3]);
}

void DriveMeasure::drawText() const {
    const auto numDrives{ m_drives.size() };

    const auto rasterX = float{ -0.95f };
    auto rasterY = float{ 1.0f };

    glColor3f(TEXT_R, TEXT_G, TEXT_B);

    for (const auto& pdi : m_drives) {

        glRasterPos2f(rasterX, rasterY);
        for (const auto c : pdi->getDriveInfoStr()) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        rasterY -= 1.0f / numDrives;
    }

}

}