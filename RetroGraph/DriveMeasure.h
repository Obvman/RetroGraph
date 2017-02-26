#pragma once

#include <vector>
#include <string>
#include <memory>

#include <GL/glew.h>

#include "DriveInfo.h"

namespace rg {

class DriveMeasure {
public:
    DriveMeasure(GLint vpX, GLint vpY, GLint vpW, GLint vpH);
    ~DriveMeasure();

    void init();
    void update(uint32_t ticks);
    void draw() const;

    /* Returns the number of fixed drives active in the system */
    size_t getNumDrives() const { return m_drives.size(); }

    /* Returns the drive list */
    const std::vector<std::unique_ptr<DriveInfo>>& getDrives() const { return m_drives; }
private:
    void drawText() const;
    void drawBar(const DriveInfo& di, float x, float y) const;

    std::vector<std::string> m_drivePaths;
    std::vector<std::unique_ptr<DriveInfo>> m_drives;

    // Rendering members
    GLint m_viewportStartX;
    GLint m_viewportStartY;
    GLint m_viewportWidth;
    GLint m_viewportHeight;
};

}