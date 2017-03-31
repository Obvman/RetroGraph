#include "../headers/DriveInfo.h"

#include "../headers/utils.h"

namespace rg {

DriveInfo::DriveInfo(char _driveLetter, uint64_t initFreeBytes,
                     uint64_t totalBytes, const char* volumeName) :
    driveLetter{ _driveLetter },
    totalFreeBytes{ initFreeBytes },
    totalBytes{ totalBytes },
    volumeName{ volumeName },
    capacityStr{} {

    const auto capacity{ totalBytes / GB };
    if (capacity < 1000) {
        capacityStr = std::to_string(capacity) + "GB";
    } else {
        char buff[6];
        snprintf(buff, sizeof(buff), "%.1fTB", capacity / 1024.0f);
        capacityStr = buff;
    }

}


DriveInfo::~DriveInfo() {
}

}
