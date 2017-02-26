#include "DriveInfo.h"

#include "utils.h"

namespace rg {

DriveInfo::DriveInfo(char _driveLetter, uint64_t initFreeBytes,
                     uint64_t totalBytes, const char* volumeName) :
    m_driveLetter{ _driveLetter },
    m_totalFreeBytes{ initFreeBytes },
    m_totalBytes{ totalBytes },
    m_volumeName{ volumeName },
    //m_driveInfoStr{},
    m_capacityStr{} {

    const auto capacity{ totalBytes / GB };
    if (capacity < 1000) {
        m_capacityStr = std::to_string(capacity) + "GB";
    } else {
        char buff[6];
        snprintf(buff, sizeof(buff), "%.1fTB", capacity / 1024.0f);
        m_capacityStr = buff;
    }

}


DriveInfo::~DriveInfo() {
}

}