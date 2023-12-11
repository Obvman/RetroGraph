export module Measures.MusicMeasure;

import Measures.Measure;

import std.core;

import "WindowsHeaderUnit.h";

namespace rg {

/* Monitors current music player status. Is dependent on ProcessMeasure, so must
 * be created after ProcessMeasure is created
 */
export class MusicMeasure : public Measure {
public:
    MusicMeasure();
    ~MusicMeasure() noexcept = default;

    /* If the player class name isn't yet set, enumerates all running windows
     * to find it. If the class name is set, then searches windows with the
     * class name as a key to determine if the window is still open or not
     */
    void update() override;
    std::chrono::microseconds updateInterval() const override { return std::chrono::seconds{ 1 }; }

    /* Returns true if the music player window is currently running */
    bool isPlayerRunning() const { return m_playerRunning; }

    bool isMusicPlaying() const { return m_isPlaying; }
    std::string_view getTrackName() const { return m_trackName; }
    std::string_view getArtist() const { return m_artist; }
    std::string_view getAlbum() const { return m_album; }
    int getElapsedTime() const { return m_elapsedTime; }
    int getTotalTime() const { return m_totalTime; }

private:
    void updateTitleString();

    void scrapeInfoFromTitle();

    bool m_playerRunning{ false };
    HWND m_playerHandle{ nullptr };
    std::string m_playerWindowTitle{ "" };

    // Current music status
    std::vector<std::string> m_titleTokens;

    bool m_isPlaying{ false };
    std::string_view m_trackName{ "" };
    std::string_view m_artist{ "" };
    std::string_view m_album{ "" };
    int m_elapsedTime{ 0U };
    int m_totalTime{ 0U };
};

} // namespace rg
