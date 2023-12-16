module Measures.Data.DriveInfo;

import Core.Units;

namespace rg {

DriveInfo::DriveInfo(char _driveLetter, uint64_t initFreeBytes, uint64_t totalBytes, const char* volumeName)
    : driveLetter{ _driveLetter }
    , totalFreeBytes{ initFreeBytes }
    , totalBytes{ totalBytes }
    , volumeName{ volumeName }
    , capacityStr{ "" } {

    updateCapacityStr();
}

void DriveInfo::updateCapacityStr() {
    const auto capacity{ bToGB(totalBytes) };
    if (capacity < 1000) {
        capacityStr = std::format("{}GB", capacity);
    } else {
        capacityStr = std::format("{:.1f}TB", capacity / 1024.0f);
    }
}

} // namespace rg
