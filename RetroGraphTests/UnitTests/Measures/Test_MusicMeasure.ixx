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
        .artist{ "My favorite artist" },
        .album{ "My favorite album" },
        .elapsedTime{ initialElapsedTime },
        .totalTime{ 50s },
    };

    auto musicDataSource{ std::make_unique<TestMusicDataSource>() };
    auto* musicDataSourceRaw{ musicDataSource.get()};
    rg::MusicMeasure measure{ testMeasureUpdateInterval, std::move(musicDataSource) };

    musicDataSourceRaw->setMusicData(testMusicData);

    SECTION("Instant update does not change data") {
        measure.update();
        REQUIRE(measure.isMusicPlaying() == defaultMusicData.isMusicPlaying);
        REQUIRE(measure.getTrackName() == defaultMusicData.trackName);
        REQUIRE(measure.getArtist() == defaultMusicData.artist);
        REQUIRE(measure.getAlbum() == defaultMusicData.album);
        REQUIRE(measure.getElapsedTime() == defaultMusicData.elapsedTime);
        REQUIRE(measure.getTotalTime() == defaultMusicData.totalTime);
    }

    SECTION("Slow update changes data") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();
        REQUIRE(measure.isMusicPlaying() == true);
        REQUIRE(measure.getTrackName() == "My favorite song" );
        REQUIRE(measure.getArtist() == "My favorite artist" );
        REQUIRE(measure.getAlbum() == "My favorite album" );
        REQUIRE(measure.getElapsedTime() == initialElapsedTime);
        REQUIRE(measure.getTotalTime() == 50s);
    }

    SECTION("Multiple update calls") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();

        SECTION("Update when no change since last update") {
            std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
            measure.update();
            REQUIRE(measure.isMusicPlaying() == true);
            REQUIRE(measure.getTrackName() == "My favorite song" );
            REQUIRE(measure.getArtist() == "My favorite artist" );
            REQUIRE(measure.getAlbum() == "My favorite album" );
            REQUIRE(measure.getElapsedTime() == initialElapsedTime);
            REQUIRE(measure.getTotalTime() == 50s);
        }

        SECTION("Update when change since last update") {
            std::this_thread::sleep_for(testMeasureUpdateInterval * 2);

            testMusicData.elapsedTime++;
            testMusicData.artist = "my least favorite artist";
            musicDataSourceRaw->setMusicData(testMusicData);
            measure.update();
            REQUIRE(measure.getElapsedTime() == seconds{ initialElapsedTime + 1s });
            REQUIRE(measure.getArtist() ==  "my least favorite artist");
        }
    }
}
