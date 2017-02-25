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

    void update(uint32_t ticks);
    void draw() const;
    void drawText() const;
private:
    std::vector<std::string> m_drivePaths;
    std::vector<std::unique_ptr<DriveInfo>> m_drives;

    // Rendering members
    GLint m_viewportStartX;
    GLint m_viewportStartY;
    GLint m_viewportWidth;
    GLint m_viewportHeight;
};

}