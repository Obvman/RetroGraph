#include "DriveInfo.h"

#include <iostream>

namespace rg {

DriveInfo::DriveInfo(char _driveLetter, uint64_t initFreeBytes,
                     uint64_t totalBytes, const char* volumeName) :
    m_driveLetter{ _driveLetter },
    m_totalFreeBytes{ initFreeBytes },
    m_totalBytes{ totalBytes },
    m_volumeName{ volumeName } {

}


DriveInfo::~DriveInfo() {
}

}