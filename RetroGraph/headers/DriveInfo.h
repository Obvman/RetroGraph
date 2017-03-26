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

    /* Sets the drive's number of available bytes */
    void setTotalFreeBytes(uint64_t b) { m_totalFreeBytes = b; }

    /* Sets the drive's total capacity in bytes. Shouldn't ever need to be
       used since fixed drives usually don't change size */
    void setTotalBytes(uint64_t b) { m_totalBytes = b; }

    /* Sets the full name of the drive */
    void setVolumeName(const std::string& vn) { m_volumeName = vn; }

    /* Gets the drive's letter according to Windows */
    char getDriveLetter() const { return m_driveLetter; }

    /* Gets the number of bytes available for use in the drive */
    uint64_t getFreeBytes() const { return m_totalFreeBytes; }

    /* Gets the number of bytes already used in the drive */
    uint64_t getUsedBytes() const { return m_totalBytes - m_totalFreeBytes; }

    /* Gets the total capacity of the drive */
    uint64_t getTotalBytes() const { return m_totalBytes; }

    /* Gets the full name of the drive */
    const std::string& getVolumeName() const { return m_volumeName; }

    /* Gets string describing the used/total space of the drive */
    const std::string& getCapacityStr() const { return m_capacityStr; }

private:
    char m_driveLetter;
    uint64_t m_totalFreeBytes;
    uint64_t m_totalBytes;
    std::string m_volumeName;
    std::string m_capacityStr;
};

}
