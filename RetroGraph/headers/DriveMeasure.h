#pragma once

#include <vector>
#include <string>
#include <memory>

#include <GL/glew.h>

#include "Measure.h"
#include "DriveInfo.h"

namespace rg {

/* Stores paths and statistics about all the system's fixed drives */
class DriveMeasure : public Measure {
public:
    DriveMeasure();
    virtual ~DriveMeasure();
    DriveMeasure(const DriveMeasure&) = delete;
    DriveMeasure& operator=(const DriveMeasure&) = delete;

    /* Enumerates the available system drives and stores initial state of each drive */
    virtual void init() override;

    /* Updates each drive with new values */
    virtual void update(uint32_t ticks) override;

    /* Returns the number of fixed drives active in the system */
    size_t getNumDrives() const { return m_drives.size(); }

    /* Returns the drive list */
    const std::vector<std::unique_ptr<DriveInfo>>& getDrives() const { return m_drives; }

private:
    std::vector<std::string> m_drivePaths;
    std::vector<std::unique_ptr<DriveInfo>> m_drives;
};

}
