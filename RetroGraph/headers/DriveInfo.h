#pragma once

#include <stdint.h>
#include <string>

namespace rg {

constexpr auto maxVolumeNameSize = uint32_t{ 64U };

/* Stores information about a single system fixed drive (SSD, HDD etc) */
class DriveInfo {
public:
    DriveInfo(char _driveLetter, uint64_t initFreeBytes, uint64_t totalBytes,
              const char* volumeName);
    ~DriveInfo();
    DriveInfo(const DriveInfo&) = delete;
    DriveInfo& operator=(const DriveInfo&) = delete;


    char driveLetter;
    uint64_t totalFreeBytes;
    uint64_t totalBytes;
    std::string volumeName;
    std::string capacityStr;
};

}
