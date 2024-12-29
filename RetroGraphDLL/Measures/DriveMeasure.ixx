export module RG.Measures:DriveMeasure;

import :Measure;

import RG.Measures.Data;
import RG.Measures.DataSources;

import std;

namespace rg {

/* Stores paths and statistics about all the system's fixed drives */
export class DriveMeasure : public Measure {
public:
    DriveMeasure(std::chrono::milliseconds updateInterval, std::unique_ptr<const IDriveDataSource> driveDataSource);
    ~DriveMeasure() noexcept = default;

    /* Returns the number of fixed drives active in the system */
    size_t getNumDrives() const { return m_driveData.drives.size(); }

    /* Returns the drive list */
    const std::vector<Drive>& getDrives() const { return m_driveData.drives; }

protected:
    /* Updates each drive with new values */
    bool updateInternal() override;

private:
    std::unique_ptr<const IDriveDataSource> m_driveDataSource;
    DriveData m_driveData;
};

} // namespace rg
