export module DriveInfo;

import <cstdint>;
import <string>;

namespace rg {

export class DriveInfo;

/* Per drive data storage container */
class DriveInfo {
public:
    DriveInfo(char _driveLetter, uint64_t initFreeBytes, uint64_t totalBytes, const char* volumeName);
    ~DriveInfo() noexcept = default;
    DriveInfo(const DriveInfo&) = default;
    DriveInfo& operator=(const DriveInfo&) = default;
    DriveInfo(DriveInfo&&) = default;
    DriveInfo& operator=(DriveInfo&&) = default;

    void updateCapacityStr();

    char driveLetter{ 'A' };
    uint64_t totalFreeBytes{ 0U };
    uint64_t totalBytes{ 0U };
    std::string volumeName{ "" };
    std::string capacityStr{ "" };
};

}
