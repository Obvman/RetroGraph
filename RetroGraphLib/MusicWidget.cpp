#include "stdafx.h"

#include "MusicWidget.h"

#include <stdio.h>
// #include <GL/glew.h>
// #include <GL/gl.h>

#include "colors.h"
#include "FontManager.h"
#include "MusicMeasure.h"
#include "RetroGraph.h"
#include "ListContainer.h"

namespace rg {

void createFormattedTimeStr(char* buffer, size_t buffSize, int seconds);

void MusicWidget::updateObservers(const RetroGraph & rg) {
    m_musicMeasure = &rg.getMusicMeasure();
}

void MusicWidget::draw() const {
    if (m_musicMeasure->isPlayerRunning()) {
        glViewport(m_viewport.x, m_viewport.y + m_viewport.height/4,
                   m_viewport.width, 3*m_viewport.height/4);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        m_fontManager->renderLine(RG_FONT_MUSIC_LARGE,
                                  m_musicMeasure->getTrackName().c_str(), 0, 0,
                                  0, 0, RG_ALIGN_TOP |
                                  RG_ALIGN_CENTERED_HORIZONTAL, 10, 30);

        m_fontManager->renderLine(RG_FONT_STANDARD,
                                  m_musicMeasure->getArtist().c_str(), 0, 0, 0,
                                  0, RG_ALIGN_CENTERED_VERTICAL |
                                  RG_ALIGN_CENTERED_HORIZONTAL);

        m_fontManager->renderLine(RG_FONT_STANDARD_BOLD,
                                  m_musicMeasure->getAlbum().c_str(), 0, 0, 0,
                                  0, RG_ALIGN_BOTTOM |
                                  RG_ALIGN_CENTERED_HORIZONTAL, 10, 30);

        const auto elapsed{ m_musicMeasure->getElapsedTime() };
        const auto total{ m_musicMeasure->getTotalTime() };

        // Convert time progress to hh:mm:ss format
        // pad seconds with leading 0s, but not minutes
        char elapsedBuff[21];
        char totalBuff[10];
        createFormattedTimeStr(elapsedBuff, sizeof(elapsedBuff), elapsed);
        createFormattedTimeStr(totalBuff, sizeof(totalBuff), total);
        strcat_s(elapsedBuff, sizeof(elapsedBuff), "/");
        strcat_s(elapsedBuff, sizeof(elapsedBuff), totalBuff);

        glViewport(m_viewport.x, m_viewport.y,
                   m_viewport.width, m_viewport.height/4);
        m_fontManager->renderLine(-0.9f, 0.5f, RG_FONT_STANDARD, elapsedBuff, 
                                  static_cast<int>(strlen(elapsedBuff)));
        drawHorizontalProgressBar(0.3f, -0.9f, 0.9f,
                                  static_cast<float>(elapsed),
                                  static_cast<float>(total));
    } else {
        m_fontManager->renderLine(RG_FONT_TIME, "No Media", 0, 0, 0, 0,
                                  RG_ALIGN_CENTERED_VERTICAL |
                                  RG_ALIGN_CENTERED_HORIZONTAL, 10, 10);
    }
}

void createFormattedTimeStr(char* buffer, size_t buffSize, int seconds) {
    if (seconds > 60 * 59) {
        const auto S{ seconds % 60 };
        const auto M{ seconds / 60 };
        const auto H{ seconds / (60 * 60) };

        snprintf(buffer, buffSize, "%d:%02d:%02d", H, M, S);
    } else if (seconds > 59) {
        const auto S{ seconds % 60 };
        const auto M{ seconds / 60 };

        snprintf(buffer, buffSize, "%02d:%02d", M, S);
    } else {
        snprintf(buffer, buffSize, "00:%02d", seconds);
    }
}

}
