#include "stdafx.h"

#include "MusicMeasure.h"

#include <cstring>
#include <iostream>
#include <cstdio>

#include "utils.h"
#include "ProcessMeasure.h"

#pragma warning(disable : 4996)

namespace rg {

MusicMeasure::MusicMeasure(const ProcessMeasure& procMeasure) :
    Measure{ 1U, 5U },
    m_processMeasure{ &procMeasure } {

    force_update();
}

void MusicMeasure::update(int ticks) {
    const auto oldTitle{ m_playerWindowTitle };

    // Get the window class name for the player if it hasn't yet been set
    // Encode the this pointer into lParam so the proc can access members
    if (ticksMatchSeconds(ticks, m_updateRates[1]) &&
        m_playerWindowClassName.empty()) {
        EnumWindows(MusicMeasure::EnumWindowsProc,
                    reinterpret_cast<LPARAM>(this));
    }

    // Check if the player window is currently open by matching the class name
    // We must validate existence of window every time before we scrape
    // title information
    m_playerHandle = FindWindow(m_playerWindowClassName.c_str(), nullptr);
    if (!m_playerHandle) {
        m_playerRunning = false;
    } else {
        m_playerRunning = true;
        updateTitleString();

        if (oldTitle != m_playerWindowTitle) {
            scrapeInfoFromTitle();
        }
    }
}

bool MusicMeasure::handleClick(int clickX, int clickY) const {
    // TODO
    (void)clickX;
    (void)clickY;
    return true;
}

void MusicMeasure::updateTitleString() {
    // Get the title string
    const auto titleLen{ GetWindowTextLength(m_playerHandle) + 1 };
    RGASSERT((titleLen - 1) != 0, "Failed to get player title length");

    m_playerWindowTitle.resize(titleLen);
    RGVERIFY(GetWindowText(m_playerHandle, &m_playerWindowTitle[0], titleLen), "Failed to get music player window title");

    // Resize to avoid double NULL terminators
    m_playerWindowTitle.resize(titleLen-1);
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
    return ticksMatchSeconds(ticks, m_updateRates.front());
}

BOOL CALLBACK MusicMeasure::EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto This{ reinterpret_cast<MusicMeasure*>(lParam) };

    char title[256];
    GetWindowText(hwnd, title, sizeof(title));

    std::string windowTitle{ title };
    if (windowTitle.find(This->m_playerTitlePattern) != std::string::npos) {
        char windowClass[256];
        GetClassName(hwnd, windowClass, sizeof(windowClass));
        This->m_playerWindowClassName = std::string{ windowClass };

        This->m_playerHandle = hwnd;
        This->m_playerRunning = true;
        This->m_playerWindowTitle = windowTitle;

        return FALSE;
    }
    return TRUE;
}

}
