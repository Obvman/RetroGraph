#pragma once

#include <vector>
#include <string>
#include <memory>

#include <GL/glew.h>

#include "DriveInfo.h"

namespace rg {

/* Stores paths and statistics about all the system's fixed drives */
class DriveMeasure {
public:
    DriveMeasure();
    ~DriveMeasure();

    /* Enumerates the available system drives and stores initial state of each drive */
    void init();

    /* Updates each drive with new values */
    void update(uint32_t ticks);

    /* Returns the number of fixed drives active in the system */
    size_t getNumDrives() const { return m_drives.size(); }

    /* Returns the drive list */
    const std::vector<std::unique_ptr<DriveInfo>>& getDrives() const { return m_drives; }

private:
    std::vector<std::string> m_drivePaths;
    std::vector<std::unique_ptr<DriveInfo>> m_drives;
};

}