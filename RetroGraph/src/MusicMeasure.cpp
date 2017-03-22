#include "../headers/MusicMeasure.h"

#include <stdio.h>

#include "../headers/ProcessMeasure.h"

namespace rg {

MusicMeasure::MusicMeasure(const ProcessMeasure* procMeasure) :
    m_playerTitlePattern{ "[foobar2000 v1." },
    m_processMeasure{ procMeasure },
    m_playerRunning{ false },
    m_playerHandle{ nullptr },
    m_playerWindowClassName{ "" } {

}

MusicMeasure::~MusicMeasure() {

}

void MusicMeasure::init() {
    EnumWindows(MusicMeasure::EnumWindowsProc, reinterpret_cast<LPARAM>(this));
}

void MusicMeasure::update(uint32_t ticks) {
    if ((ticks % (ticksPerSecond * 5)) == 0) {
        // Get the window class name for the player if it hasn't yet been set
        if (m_playerWindowClassName.size() == 0) {
            // Encode the this pointer into lParam so the proc can access members
            EnumWindows(MusicMeasure::EnumWindowsProc, reinterpret_cast<LPARAM>(this));
        } else {
            // Check if the player window is currently open by matching the class name
            m_playerHandle = FindWindow(m_playerWindowClassName.c_str(), nullptr);
            if (!m_playerHandle) {
                m_playerRunning = false;
            } else {
                m_playerRunning = true;
            }
        }
    }
}

BOOL CALLBACK MusicMeasure::EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto This{ reinterpret_cast<MusicMeasure*>(lParam) };

    char title[256];
    char windowClass[256];
    GetClassName(hwnd, windowClass, sizeof(windowClass));
    GetWindowText(hwnd, title, sizeof(title));

    std::string className{ title };
    if (className.find(This->m_playerTitlePattern) != std::string::npos) {
        This->m_playerWindowClassName = std::string{ windowClass };
        This->m_playerHandle = hwnd;
        This->m_playerRunning = true;

        return FALSE;
    }
    return TRUE;
}

}
