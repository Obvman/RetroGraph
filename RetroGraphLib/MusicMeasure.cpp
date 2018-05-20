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
    m_processMeasure{ &procMeasure } {

    update(0);
}

void MusicMeasure::update(uint32_t ticks) {
    if ((ticks % (ticksPerSecond * 1)) == 0) {
        const auto oldTitle{ m_playerWindowTitle };

        // Get the window class name for the player if it hasn't yet been set
        // Encode the this pointer into lParam so the proc can access members
        if ((ticks % (ticksPerSecond * 5)) == 0 &&
            (m_playerWindowClassName.size() == 0)) {
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
}

bool MusicMeasure::handleClick(int32_t clickX, int32_t clickY) const {
    // TODO
    (void)clickX;
    (void)clickY;
    return true;
}

void MusicMeasure::updateTitleString() {
    // Get the title string
    const auto titleLen{ GetWindowTextLength(m_playerHandle) + 1 };
    if ((titleLen - 1) == 0) {
        fatalMessageBox("Failed to get player title length");
    } 

    m_playerWindowTitle.resize(titleLen);
    if (!GetWindowText(m_playerHandle, &m_playerWindowTitle[0], titleLen)) {
        fatalMessageBox("Failed to get music player window title");
    }
    // Resize to avoid double NULL terminators
    m_playerWindowTitle.resize(titleLen-1);
}

void MusicMeasure::scrapeInfoFromTitle() {
    std::vector<std::string> tokens{};

    char title[256];
    // Copy and null-terminate
    title[m_playerWindowTitle.copy(title, sizeof(title), 0)] = '\0';
    const char* delim{ "|" };
    char* nextToken{ nullptr };
    char* whitespace{ "\n \t" };

    char* token{ strtok_s(title, delim, &nextToken) };
    // Split each section (deliminated by |) of the title into the token vector
    while (token) {
        auto tokenStr = std::string{ token };
        // Trim leading whitespace and add to the token vector
        const auto begin{ tokenStr.find_first_not_of(whitespace) };
        if (begin != std::string::npos) {
            tokens.emplace_back(tokenStr.substr(begin, tokenStr.size()));
        } else {
            // Empty regions (e.g. song with no album/artist)
            tokens.emplace_back("-");
        }

        token = strtok_s(nullptr, delim, &nextToken);
    }

    // The last element of the vector contains the player version which we don't
    // need, so pop it off
    tokens.pop_back();

    // Handle case where player is open but no song is playing
    if (tokens.size() == 0) {
        m_isPlaying = false;
        m_trackName = "-";
        m_artist = "-";
        m_album = "-";
        return;
    }

    // token ordering of: track name, artist, album+track number, playstate
    m_trackName = tokens[0];

    // Handle non-existing cases for album/artist
    if (tokens[1] == "-") {
        m_artist.clear();
    } else {
        m_artist = tokens[1];
    }
    if (tokens[2] == "-") {
        m_album.clear();
    } else {
        m_album = tokens[2];
    }

    if (tokens[3] == "Playing") {
        m_isPlaying = true;
    } else {
        m_isPlaying = false;
    }

    auto& timeProgress{ tokens[4] };
    const char* elapsed = strtok_s(&timeProgress[0], ",", &nextToken);
    const char* total = strtok_s(nullptr, ",", &nextToken);
    m_elapsedTime = std::stoi(elapsed);
    m_totalTime = std::stoi(total);

}

BOOL CALLBACK MusicMeasure::EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto This{ reinterpret_cast<MusicMeasure*>(lParam) };

    char title[256];
    char windowClass[256];
    GetClassName(hwnd, windowClass, sizeof(windowClass));
    GetWindowText(hwnd, title, sizeof(title));

    std::string windowTitle{ title };
    if (windowTitle.find(This->m_playerTitlePattern) != std::string::npos) {
        This->m_playerWindowClassName = std::string{ windowClass };
        This->m_playerHandle = hwnd;
        This->m_playerRunning = true;
        This->m_playerWindowTitle = windowTitle;

        return FALSE;
    }
    return TRUE;
}

}