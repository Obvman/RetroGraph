#pragma once

#include <string>
#include <stdint.h>
#include <Windows.h>

#include "Measure.h"
#include "drawUtils.h"

namespace rg {

class ProcessMeasure;

/* Monitors current music player status. Is dependent on ProcessMeasure, so must
 * be created after ProcessMeasure is created
 */
class MusicMeasure : public Measure {
public:
    MusicMeasure(const ProcessMeasure* procMeasure) : 
        m_processMeasure{ procMeasure } {}
    virtual ~MusicMeasure() noexcept = default;
    MusicMeasure(const MusicMeasure&) = delete;
    MusicMeasure& operator=(const MusicMeasure&) = delete;

    /* Searches all running windows for the music player. */
    void init() override;

    /* If the player class name isn't yet set, enumerates all running windows
     * to find it. If the class name is set, then searches windows with the
     * class name as a key to determine if the window is still open or not
     */
    void update(uint32_t ticks) override;

    /* Returns true if the music player window is currently running */
    bool isPlayerRunning() const { return m_playerRunning; }

    bool isMusicPlaying() const { return m_isPlaying; }
    const std::string& getTrackName() const { return m_trackName; }
    const std::string& getArtist() const { return m_artist; }
    const std::string& getAlbum() const { return m_album; }
    uint32_t getElapsedTime() const { return m_elapsedTime; }
    uint32_t getTotalTime() const { return m_totalTime; }
private:
    /* Called for each window running in the operating system. Tries to find
     * the music player by matching against the window title. If found, 
     * sets the player class name member
     */
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

    void updateTitleString();

    void scrapeInfoFromTitle();


    const char* m_playerTitlePattern{ "foobar2000 v1.3." };

    const ProcessMeasure* m_processMeasure{ nullptr };

    bool m_playerRunning{ false };
    HWND m_playerHandle{ nullptr };
    std::string m_playerWindowClassName{ "" };
    std::string m_playerWindowTitle{ "" };

    // Current music status
    bool m_isPlaying{ false };
    std::string m_trackName{ "" };
    std::string m_artist{ "" };
    std::string m_album{ "" };
    uint32_t m_elapsedTime{ 0U };
    uint32_t m_totalTime{ 0U };
};
}