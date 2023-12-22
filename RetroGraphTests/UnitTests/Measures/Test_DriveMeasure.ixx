export module UnitTests.Test_DriveMeasure;

import RG.Core;
import RG.Measures;
import RG.Measures.DataSources;

import std.core;

import "Catch2HeaderUnit.h";

using namespace std::chrono;

constexpr milliseconds testMeasureUpdateInterval{ 10 };

export class TestDriveDataSource : public rg::IDriveDataSource {
public:
    rg::DriveData getDriveData() const override { return m_driveData; }

    void setDriveData(const rg::DriveData& driveData) { m_driveData = driveData; }

    rg::DriveData m_driveData{};
};

TEST_CASE("Measures::DriveMeasure. Update", "[measure]") {
    constexpr seconds initialElapsedTime{ 10 };
    const rg::DriveData defaultDriveData{};
    rg::DriveData testDriveData{
        .drives{{ 'C', 2 * rg::GB, 10 * rg::GB, "Test drive one" },
                { 'D', 40 * rg::GB, 100 * rg::GB, "Test drive two" }}
    };

    auto driveDataSource{ std::make_unique<TestDriveDataSource>() };
    auto* driveDataSourceRaw{ driveDataSource.get() };
    rg::DriveMeasure measure{ testMeasureUpdateInterval, std::move(driveDataSource) };

    driveDataSourceRaw->setDriveData(testDriveData);

    SECTION("Instant update does not change data") {
        measure.update();
        REQUIRE(measure.getNumDrives() == 0);
    }

    SECTION("Slow update changes data") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();
        REQUIRE(measure.getNumDrives() == 2);
        REQUIRE(measure.getDrives()[0].driveLetter == 'C');
        REQUIRE(measure.getDrives()[0].totalFreeBytes == 2 * rg::GB);
        REQUIRE(measure.getDrives()[0].totalBytes == 10 * rg::GB);
        REQUIRE(measure.getDrives()[0].volumeName == "Test drive one");
        REQUIRE(measure.getDrives()[1].driveLetter == 'D');
        REQUIRE(measure.getDrives()[1].totalFreeBytes == 40 * rg::GB);
        REQUIRE(measure.getDrives()[1].totalBytes == 100 * rg::GB);
        REQUIRE(measure.getDrives()[1].volumeName == "Test drive two");
    }

    SECTION("Multiple update calls") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);

        SECTION("Update when no change since last update") {
            measure.update();

            REQUIRE(measure.getNumDrives() == 2);
            REQUIRE(measure.getDrives()[0].driveLetter == 'C');
            REQUIRE(measure.getDrives()[0].totalFreeBytes == 2 * rg::GB);
            REQUIRE(measure.getDrives()[0].totalBytes == 10 * rg::GB);
            REQUIRE(measure.getDrives()[0].volumeName == "Test drive one");
            REQUIRE(measure.getDrives()[1].driveLetter == 'D');
            REQUIRE(measure.getDrives()[1].totalFreeBytes == 40 * rg::GB);
            REQUIRE(measure.getDrives()[1].totalBytes == 100 * rg::GB);
            REQUIRE(measure.getDrives()[1].volumeName == "Test drive two");
        }

        SECTION("Update when change since last update (Modified drive)") {
            testDriveData.drives[0].totalFreeBytes = 1 * rg::GB;
            driveDataSourceRaw->setDriveData(testDriveData);
            measure.update();

            REQUIRE(measure.getNumDrives() == 2);
            REQUIRE(measure.getDrives()[0].driveLetter == 'C');
            REQUIRE(measure.getDrives()[0].totalFreeBytes == 1 * rg::GB);
            REQUIRE(measure.getDrives()[0].totalBytes == 10 * rg::GB);
            REQUIRE(measure.getDrives()[0].volumeName == "Test drive one");
            REQUIRE(measure.getDrives()[1].driveLetter == 'D');
            REQUIRE(measure.getDrives()[1].totalFreeBytes == 40 * rg::GB);
            REQUIRE(measure.getDrives()[1].totalBytes == 100 * rg::GB);
            REQUIRE(measure.getDrives()[1].volumeName == "Test drive two");
        }

        SECTION("Update when change since last update (Added drive)") {
            testDriveData.drives.emplace_back('E', 1 * rg::MB, 4 * rg::MB, "Test inserted drive");

            driveDataSourceRaw->setDriveData(testDriveData);
            measure.update();

            REQUIRE(measure.getNumDrives() == 3);
            REQUIRE(measure.getDrives()[0].driveLetter == 'C');
            REQUIRE(measure.getDrives()[0].totalFreeBytes == 2 * rg::GB);
            REQUIRE(measure.getDrives()[0].totalBytes == 10 * rg::GB);
            REQUIRE(measure.getDrives()[0].volumeName == "Test drive one");
            REQUIRE(measure.getDrives()[1].driveLetter == 'D');
            REQUIRE(measure.getDrives()[1].totalFreeBytes == 40 * rg::GB);
            REQUIRE(measure.getDrives()[1].totalBytes == 100 * rg::GB);
            REQUIRE(measure.getDrives()[1].volumeName == "Test drive two");
            REQUIRE(measure.getDrives()[2].driveLetter == 'E');
            REQUIRE(measure.getDrives()[2].totalFreeBytes == 1 * rg::MB);
            REQUIRE(measure.getDrives()[2].totalBytes == 4 * rg::MB);
            REQUIRE(measure.getDrives()[2].volumeName == "Test inserted drive");
        }

        SECTION("Update when change since last update (Removed drive)") {
            testDriveData.drives.pop_back();
            driveDataSourceRaw->setDriveData(testDriveData);
            measure.update();

            REQUIRE(measure.getNumDrives() == 1);
            REQUIRE(measure.getDrives()[0].driveLetter == 'C');
            REQUIRE(measure.getDrives()[0].totalFreeBytes == 2 * rg::GB);
            REQUIRE(measure.getDrives()[0].totalBytes == 10 * rg::GB);
            REQUIRE(measure.getDrives()[0].volumeName == "Test drive one");
        }
    }
}
