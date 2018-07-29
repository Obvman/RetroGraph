#pragma once

#include "stdafx.h"

#include <string>
#include <cstdint>
#include <Windows.h>

#include "drawUtils.h"
#include "Measure.h"

namespace rg {

class ProcessMeasure;

/* Monitors current music player status. Is dependent on ProcessMeasure, so must
 * be created after ProcessMeasure is created
 */
class MusicMeasure : public Measure {
public:
    MusicMeasure(const ProcessMeasure& procMeasure);
    ~MusicMeasure() noexcept = default;

    /* If the player class name isn't yet set, enumerates all running windows
     * to find it. If the class name is set, then searches windows with the
     * class name as a key to determine if the window is still open or not
     */
    void update(int ticks) override;

    /* Checks click coordinates for collision with media controls, returns true
     * if a media key was successfully clicked
     */
    bool handleClick(int clickX, int clickY) const;

    /* Returns true if the music player window is currently running */
    bool isPlayerRunning() const { return m_playerRunning; }

    bool isMusicPlaying() const { return m_isPlaying; }
    std::string_view getTrackName() const { return m_trackName; }
    std::string_view getArtist() const { return m_artist; }
    std::string_view getAlbum() const { return m_album; }
    int getElapsedTime() const { return m_elapsedTime; }
    int getTotalTime() const { return m_totalTime; }

    void refreshSettings() override { }
private:
    bool shouldUpdate(int ticks) const override;

    /* Called for each window running in the operating system. Tries to find
     * the music player by matching against the window title. If found,
     * sets the player class name member
     */
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

    void updateTitleString();

    void scrapeInfoFromTitle();


    std::string_view m_playerTitlePattern{ "foobar2000 v1.\0" };

    const ProcessMeasure* m_processMeasure{ nullptr };

    bool m_playerRunning{ false };
    HWND m_playerHandle{ nullptr };
    std::string m_playerWindowClassName{ "" };
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
