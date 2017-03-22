#include "../headers/MusicMeasure.h"

#include <stdio.h>

#include "../headers/ProcessMeasure.h"

namespace rg {

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

MusicMeasure::MusicMeasure(const ProcessMeasure* procMeasure) :
    m_processMeasure{ procMeasure },
    m_playerRunning{ false } {

}

MusicMeasure::~MusicMeasure() {

}

void MusicMeasure::init() {
}

void MusicMeasure::update(uint32_t ticks) {
    if ((ticks % (ticksPerSecond * 5)) == 0) {
        /* // Check if foobar is running in the process list */
        /* const auto foobarPID{ m_processMeasure->getPIDFromName("foobar2000") }; */
        /* if (foobarPID < 0) { */
        /*     printf("Foobar is not running\n"); */
        /*     m_playerRunning = false; */
        /* } else { */
        /*     printf("Foobar is running\n"); */
        /*     m_playerRunning = true; */

        /*     // Make check if the PID has changed since the last update, if so, */
        /*     // We'll need to find the window handle again */
        /*     if (foobarPID != m_playerPID) { */
        /*         // Find window handle */
        /*     } */

        /*     m_playerPID = foobarPID; */
        /* } */
        // Enumerate all the currently open windows, searching for the music player
        EnumWindows(EnumWindowsProc, 0);
    }
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM) {
    char title[80];
    char windowClass[80];
    GetClassName(hwnd, windowClass, sizeof(windowClass));
    GetWindowText(hwnd, title, sizeof(title));

    //printf("%s\n", title);
    //printf("%s\n", windowClass);
    //fflush(stdout);

    return TRUE;
}

}
