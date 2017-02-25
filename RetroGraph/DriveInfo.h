#pragma once

#include <stdint.h>
#include <string>

namespace rg {

constexpr uint32_t maxVolumeNameSize{ 64U };

class DriveInfo {
public:
    DriveInfo(char _driveLetter, uint64_t initFreeBytes, uint64_t totalBytes,
              const char* volumeName);
    ~DriveInfo();

    void setTotalFreeBytes(uint64_t b) { m_totalFreeBytes = b; }
    void setTotalBytes(uint64_t b) { m_totalBytes = b; }
    void setVolumeName(const std::string& vn) { m_volumeName = vn; }
    void setDriveInfoStr(const std::string& is) { m_driveInfoStr = is; }

    char getDriveLetter() const { return m_driveLetter; }
    uint64_t getFreeBytes() const { return m_totalFreeBytes; }
    uint64_t getTotalBytes() const { return m_totalBytes; }
    const std::string& getVolumeName() const { return m_volumeName; }
    const std::string& getDriveInfoStr() const { return m_driveInfoStr; }

private:
    char m_driveLetter;
    uint64_t m_totalFreeBytes;
    uint64_t m_totalBytes;
    std::string m_volumeName;
    std::string m_driveInfoStr;
};

}