module Measures.DataSources.FoobarMusicDataSource;

import RG.Core;

import Measures.DataSources.IMusicDataSource;

import "RGAssert.h";
import "WindowsHeaderUnit.h";

namespace rg {

constexpr const char * foobarWindowClassName{ "{97E27FAA-C0B3-4b8e-A693-ED7881E99FC1}" };

MusicData FoobarMusicDataSource::getMusicData() const {
    // Check if the player window is currently open by matching the class name
    // We must validate existence of window every time before we scrape
    // title information
    const auto playerHandle{ FindWindow(foobarWindowClassName, nullptr) };
    if (!playerHandle) {
        m_musicDataCache.isMusicPlayerRunning = false;
    } else {
        m_musicDataCache.isMusicPlayerRunning = true;

        const auto playerWindowTitle{ getFoobarWindowTitle(playerHandle) };

        if (m_playerWindowTitleCache != playerWindowTitle) {
            m_playerWindowTitleCache = playerWindowTitle;
            populateDataFromTitle(playerWindowTitle, m_musicDataCache);
        }
    }

    return m_musicDataCache;
}

std::string FoobarMusicDataSource::getFoobarWindowTitle(HWND playerHandle) const {
    // Get the title string
    const auto titleLen{ GetWindowTextLength(playerHandle) + 1 };
    RGASSERT((titleLen - 1) > 0, "Failed to get player title length");

    std::string playerWindowTitle;
    playerWindowTitle.resize(titleLen);
    RGVERIFY(GetWindowText(playerHandle, &playerWindowTitle[0], titleLen), "Failed to get music player window title");

    // Resize to avoid double NULL terminators
    playerWindowTitle.resize(static_cast<size_t>(titleLen) - 1);
    return playerWindowTitle;
}

void FoobarMusicDataSource::populateDataFromTitle(const std::string& playerWindowTitle,
                                                  MusicData& musicData) const {
    std::vector<std::string> titleTokens;

    char title[256];
    // Copy and null-terminate
    title[playerWindowTitle.copy(title, sizeof(title), 0)] = '\0';
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
            titleTokens.emplace_back(tokenStr.substr(begin, tokenStr.size()));
        } else {
            // Empty regions (e.g. song with no album/artist)
            titleTokens.emplace_back("-");
        }

        token = strtok_s(nullptr, delim, &nextToken);
    }

    // The last element of the vector contains the player version which we don't
    // need, so pop it off
    titleTokens.pop_back();

    // Handle case where player is open but no song is playing
    if (titleTokens.empty()) {
        musicData.isMusicPlaying = false;
        musicData.trackName = "-";
        musicData.artist = "-";
        musicData.album = "-";
        return;
    }

    // token ordering of: track name, artist, album+track number, playstate
    musicData.trackName = titleTokens[0];

    // Handle non-existing cases for album/artist
    if (titleTokens[1] == "-") {
        musicData.artist = "";
    } else {
        musicData.artist = titleTokens[1];
    }

    if (titleTokens[2] == "-") {
        musicData.album = "";
    } else {
        musicData.album = titleTokens[2];
    }

    musicData.isMusicPlaying = bool{ titleTokens[3] == "Playing" };

    auto& timeProgress{ titleTokens[4] };
    std::string_view elapsed = strtok_s(&timeProgress[0], ",", &nextToken);
    std::string_view total = strtok_s(nullptr, ", ", &nextToken);
    musicData.elapsedTime = strToNum<decltype(musicData.elapsedTime)>(elapsed);
    musicData.totalTime = strToNum<decltype(musicData.totalTime)>(total);
}

}
