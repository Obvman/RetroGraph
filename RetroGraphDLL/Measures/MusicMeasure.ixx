module;

#include "RGAssert.h"

export module Measures.MusicMeasure;

import Utils;

import Measures.Measure;
import Measures.ProcessMeasure;

import std.core;

import "WindowsHeaders.h";

namespace rg {

constexpr const char * foobarWindowClassName{ "{97E27FAA-C0B3-4b8e-A693-ED7881E99FC1}" };

/* Monitors current music player status. Is dependent on ProcessMeasure, so must
 * be created after ProcessMeasure is created
 */
export class MusicMeasure : public Measure {
public:
    MusicMeasure(const ProcessMeasure& procMeasure);
    ~MusicMeasure() noexcept = default;

    /* If the player class name isn't yet set, enumerates all running windows
     * to find it. If the class name is set, then searches windows with the
     * class name as a key to determine if the window is still open or not
     */
    void update(int ticks) override;

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

    void updateTitleString();

    void scrapeInfoFromTitle();


    const ProcessMeasure* m_processMeasure{ nullptr };

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

MusicMeasure::MusicMeasure(const ProcessMeasure& procMeasure)
    : m_processMeasure{ &procMeasure } {

    force_update();
}

void MusicMeasure::update(int /*ticks*/) {

    // Check if the player window is currently open by matching the class name
    // We must validate existence of window every time before we scrape
    // title information
    m_playerHandle = FindWindow(foobarWindowClassName, nullptr);
    if (!m_playerHandle) {
        m_playerRunning = false;
    } else {
        m_playerRunning = true;

        const auto oldTitle{ m_playerWindowTitle };

        updateTitleString();

        if (oldTitle != m_playerWindowTitle) {
            scrapeInfoFromTitle();
        }
    }
}

void MusicMeasure::updateTitleString() {
    // Get the title string
    const auto titleLen{ GetWindowTextLength(m_playerHandle) + 1 };
    RGASSERT((titleLen - 1) > 0, "Failed to get player title length");

    m_playerWindowTitle.resize(titleLen);
    RGVERIFY(GetWindowText(m_playerHandle, &m_playerWindowTitle[0], titleLen), "Failed to get music player window title");

    // Resize to avoid double NULL terminators
    m_playerWindowTitle.resize(static_cast<size_t>(titleLen) - 1);
}

void MusicMeasure::scrapeInfoFromTitle() {
    m_titleTokens.clear();

    char title[256];
    // Copy and null-terminate
    title[m_playerWindowTitle.copy(title, sizeof(title), 0)] = '\0';
    const char* delim{ "|" };
    char* nextToken{ nullptr };
    const char* whitespace{ "\n \t" };

    // Split each section (deliminated by |) of the title into the token vector
    char* token{ strtok_s(title, delim, &nextToken) };
    while (token) {
        auto tokenStr = std::string_view{ token };

        // Trim leading whitespace and add to the token vector
        const auto begin{ tokenStr.find_first_not_of(whitespace) };
        if (begin != std::string::npos) {
            m_titleTokens.emplace_back(tokenStr.substr(begin, tokenStr.size()));
        } else {
            // Empty regions (e.g. song with no album/artist)
            m_titleTokens.emplace_back("-");
        }

        token = strtok_s(nullptr, delim, &nextToken);
    }

    // The last element of the vector contains the player version which we don't
    // need, so pop it off
    m_titleTokens.pop_back();

    // Handle case where player is open but no song is playing
    if (m_titleTokens.empty()) {
        m_isPlaying = false;
        m_trackName = "-";
        m_artist = "-";
        m_album = "-";
        return;
    }

    // token ordering of: track name, artist, album+track number, playstate
    m_trackName = m_titleTokens[0];

    // Handle non-existing cases for album/artist
    if (m_titleTokens[1] == "-") {
        m_artist = "";
    } else {
        m_artist = m_titleTokens[1];
    }

    if (m_titleTokens[2] == "-") {
        m_album = "";
    } else {
        m_album = m_titleTokens[2];
    }

    m_isPlaying = bool{ m_titleTokens[3] == "Playing" };

    auto& timeProgress{ m_titleTokens[4] };
    std::string_view elapsed = strtok_s(&timeProgress[0], ",", &nextToken);
    std::string_view total = strtok_s(nullptr, ", ", &nextToken);
    m_elapsedTime = strToNum<decltype(m_elapsedTime)>(elapsed);
    m_totalTime = strToNum<decltype(m_totalTime)>(total);
}

bool MusicMeasure::shouldUpdate(int ticks) const {
    return ticksMatchSeconds(ticks, 1);
}

} // namespace rg
