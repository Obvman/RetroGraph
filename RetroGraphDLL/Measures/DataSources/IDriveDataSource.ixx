export module RG.Measures.DataSources:IDriveDataSource;

import std.core;

namespace rg {

/* Per drive data storage container */
export struct Drive {

    Drive(char _driveLetter, uint64_t initFreeBytes, uint64_t totalBytes, const char* volumeName)
        : driveLetter{ _driveLetter }
        , totalFreeBytes{ initFreeBytes }
        , totalBytes{ totalBytes }
        , volumeName{ volumeName } {
    }

    auto operator<=>(const Drive&) const = default;

    char driveLetter;
    uint64_t totalFreeBytes;
    uint64_t totalBytes;
    std::string volumeName;
};

export struct DriveData {
    std::vector<Drive> drives;

    auto operator<=>(const DriveData&) const = default;
};

export class IDriveDataSource {
public:
    virtual ~IDriveDataSource() = default;

    virtual DriveData getDriveData() const = 0;
};

}
