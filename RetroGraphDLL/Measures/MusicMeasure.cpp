module Measures.MusicMeasure;

import Core.Strings;

import "RGAssert.h";
import "WindowsHeaderUnit.h";

namespace rg {

constexpr const char * foobarWindowClassName{ "{97E27FAA-C0B3-4b8e-A693-ED7881E99FC1}" };

MusicMeasure::MusicMeasure() {
    updateInternal();
}

bool MusicMeasure::updateInternal() {
    // Check if the player window is currently open by matching the class name
    // We must validate existence of window every time before we scrape
    // title information
    m_playerHandle = FindWindow(foobarWindowClassName, nullptr);
    bool playerRunningStateChanged{ false };
    bool playerTitleChanged{ false };
    if (!m_playerHandle) {
        if (m_playerRunning)
            playerRunningStateChanged = true;

        m_playerRunning = false;
    } else {
        if (!m_playerRunning)
            playerRunningStateChanged = true;

        m_playerRunning = true;

        const auto oldTitle{ m_playerWindowTitle };

        updateTitleString();

        if (oldTitle != m_playerWindowTitle) {
            scrapeInfoFromTitle();
            playerTitleChanged = true;
        }
    }

    return (playerRunningStateChanged || playerTitleChanged);
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

} // namespace rg
