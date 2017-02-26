#pragma once

#include <vector>
#include <string>
#include <memory>

#include <GL/glew.h>

#include "DriveInfo.h"

namespace rg {

class DriveMeasure {
public:
    DriveMeasure();
    ~DriveMeasure();

    void init();
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