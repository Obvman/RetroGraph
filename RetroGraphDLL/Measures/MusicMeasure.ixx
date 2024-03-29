export module RG.Measures:MusicMeasure;

import :Measure;

import RG.Measures.DataSources;

import std.core;

namespace rg {

/* Monitors current music player status. Is dependent on ProcessMeasure, so must
 * be created after ProcessMeasure is created
 */
export class MusicMeasure : public Measure {
public:
    MusicMeasure(std::chrono::milliseconds updateInterval, std::unique_ptr<const IMusicDataSource> musicDataSource);
    ~MusicMeasure() noexcept = default;

    bool isPlayerRunning() const { return m_musicData.isMusicPlayerRunning; }

    bool isMusicPlaying() const { return m_musicData.isMusicPlaying; }
    std::string_view getTrackName() const { return m_musicData.trackName; }
    std::string_view getArtist() const { return m_musicData.artist; }
    std::string_view getAlbum() const { return m_musicData.album; }
    std::chrono::seconds getElapsedTime() const { return m_musicData.elapsedTime; }
    std::chrono::seconds getTotalTime() const { return m_musicData.totalTime; }

protected:
    /* If the player class name isn't yet set, enumerates all running windows
     * to find it. If the class name is set, then searches windows with the
     * class name as a key to determine if the window is still open or not
     */
    bool updateInternal() override;

private:
    std::unique_ptr<const IMusicDataSource> m_musicDataSource;
    MusicData m_musicData;
};

} // namespace rg
