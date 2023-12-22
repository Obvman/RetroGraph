module RG.Measures:DriveMeasure;

import "WindowsHeaderUnit.h";

namespace rg {

constexpr auto axVolumeNameSize = int{ 64U };

DriveMeasure::DriveMeasure(std::chrono::milliseconds updateInterval,
                           std::unique_ptr<const IDriveDataSource> driveDataSource)
    : Measure{ updateInterval }
    , m_driveDataSource{ std::move(driveDataSource) }
    , m_driveData{ m_driveDataSource->getDriveData() } {}

bool DriveMeasure::updateInternal() {
    const DriveData oldDriveData{ m_driveData };
    m_driveData = m_driveDataSource->getDriveData();
    return oldDriveData != m_driveData;
}

} // namespace rg
