export module UnitTests.Test_MusicMeasure;

import RG.Measures;
import RG.Measures.DataSources;

import std.core;

import "Catch2HeaderUnit.h";

using namespace std::chrono;

constexpr milliseconds testMeasureUpdateInterval{ 10 };

export class TestMusicDataSource : public rg::IMusicDataSource {
public:
    rg::MusicData getMusicData() const override { return m_musicData; }

    void setMusicData(const rg::MusicData& musicData) { m_musicData = musicData; }

    rg::MusicData m_musicData{};
};

TEST_CASE("Measures::MusicMeasure. Update", "[measure]") {
    constexpr seconds initialElapsedTime{ 10 };
    const rg::MusicData defaultMusicData{};
    rg::MusicData testMusicData{
        .isMusicPlayerRunning{ true },
        .isMusicPlaying{ true },
        .trackName{ "My favorite song" },
        .artist{ "My favorite to artist" },
        .album{ "My favorite to album" },
        .elapsedTime{ initialElapsedTime },
        .totalTime{ 50 },
    };

    auto musicDataSource{ std::make_unique<TestMusicDataSource>() };
    auto* musicDataSourceRaw{ musicDataSource.get()};
    rg::MusicMeasure measure{ testMeasureUpdateInterval, std::move(musicDataSource) };

    musicDataSourceRaw->setMusicData(testMusicData);
    bool updateEventTriggered{ false };
    auto handle{ measure.postUpdate.attach([&]() { updateEventTriggered = true; }) };

    SECTION("Instant update does not trigger event") {
        measure.update();
        REQUIRE(!updateEventTriggered);
        REQUIRE(measure.isPlayerRunning() == defaultMusicData.isMusicPlayerRunning);
        REQUIRE(measure.isMusicPlaying() == defaultMusicData.isMusicPlaying);
        REQUIRE(measure.getTrackName() == defaultMusicData.trackName);
        REQUIRE(measure.getArtist() == defaultMusicData.artist);
        REQUIRE(measure.getAlbum() == defaultMusicData.album);
        REQUIRE(measure.getElapsedTime() == defaultMusicData.elapsedTime);
        REQUIRE(measure.getTotalTime() == defaultMusicData.totalTime);
    }

    SECTION("Slow update triggers event") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();
        REQUIRE(updateEventTriggered);
        REQUIRE(measure.isPlayerRunning() == true);
        REQUIRE(measure.isMusicPlaying() == true);
        REQUIRE(measure.getTrackName() == "My favorite song" );
        REQUIRE(measure.getArtist() == "My favorite to artist" );
        REQUIRE(measure.getAlbum() == "My favorite to album" );
        REQUIRE(measure.getElapsedTime() == seconds{ 10 });
        REQUIRE(measure.getTotalTime() == seconds{ 50 });
    }

    SECTION("Multiple update calls") {
        measure.update();
        REQUIRE(!updateEventTriggered);

        updateEventTriggered = false;
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();
        REQUIRE(updateEventTriggered);

        updateEventTriggered = false;
        measure.update();
        REQUIRE(!updateEventTriggered);

        SECTION("Update when no change since last update") {
            // Update should be done but nothing changed, so no event
            updateEventTriggered = false;
            std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
            measure.update();
            REQUIRE(!updateEventTriggered);
        }

        SECTION("Update when change since last update") {
            updateEventTriggered = false;
            std::this_thread::sleep_for(testMeasureUpdateInterval * 2);

            testMusicData.elapsedTime++;
            musicDataSourceRaw->setMusicData(testMusicData);
            measure.update();
            REQUIRE(updateEventTriggered);
            REQUIRE(measure.getElapsedTime() == seconds{ initialElapsedTime.count() + 1 });
        }
    }

    measure.postUpdate.detach(handle);
}
