#include "../headers/MusicWidget.h"

#include <stdio.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include "../headers/colors.h"
#include "../headers/FontManager.h"
#include "../headers/MusicMeasure.h"

namespace rg {

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

MusicWidget::MusicWidget() {

}

MusicWidget::~MusicWidget() {

}

void MusicWidget::init(const FontManager* fontManager, const MusicMeasure* musicMeasure,
                         Viewport viewport) {

    m_fontManager = fontManager;
    m_viewport = viewport;
    m_musicMeasure = musicMeasure;
}

void MusicWidget::draw() const {
    glViewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);

    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);

    if (m_musicMeasure->isPlayerRunning()) {
        glViewport(m_viewport.x, m_viewport.y + m_viewport.height/4,
                   m_viewport.width, 3*m_viewport.height/4);

        m_fontManager->renderLine(RG_FONT_MUSIC_LARGE, 
                m_musicMeasure->getTrackName().c_str(),
                0, 0, 0, 0, RG_ALIGN_TOP | RG_ALIGN_CENTERED_HORIZONTAL,
                10, 30);

        m_fontManager->renderLine(RG_FONT_MUSIC,
                m_musicMeasure->getArtist().c_str(),
                0, 0, 0, 0, RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_CENTERED_HORIZONTAL);

        m_fontManager->renderLine(RG_FONT_MUSIC,
                m_musicMeasure->getAlbum().c_str(),
                0, 0, 0, 0, RG_ALIGN_BOTTOM | RG_ALIGN_CENTERED_HORIZONTAL,
                10, 30);

        // Draw the progression bar
        glViewport(m_viewport.x, m_viewport.y,
                   m_viewport.width, m_viewport.height/4);
        drawProgressBar();
    } else {
        m_fontManager->renderLine(RG_FONT_TIME, "No Media", 0, 0, 0, 0,
                RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_CENTERED_HORIZONTAL, 10, 10);
    }
}

void MusicWidget::drawProgressBar() const {
    const auto elapsed{ m_musicMeasure->getElapsedTime() };
    const auto total{ m_musicMeasure->getTotalTime() };

    const auto percentage = float{ (static_cast<float>(elapsed) / total) };
    constexpr auto barWidth = float{ 0.3f };
    constexpr auto startX = float{ -0.8f };
    constexpr auto endX = float{ 0.8f };
    constexpr auto rangeX{ endX - startX };
    const float barStartY{ ((2.0f - barWidth) / 2.0f) - 1.0f };

    glBegin(GL_QUADS); {
        glColor3f(BARFILLED_R, BARFILLED_G, BARFILLED_B);
        glVertex2f(startX,                       barStartY);
        glVertex2f(startX,                       barStartY + barWidth);
        glVertex2f(startX + percentage * rangeX, barStartY + barWidth);
        glVertex2f(startX + percentage * rangeX, barStartY);

        glColor3f(BARFREE_R, BARFREE_G, BARFREE_B);
        glVertex2f(startX + percentage * rangeX, barStartY);
        glVertex2f(startX + percentage * rangeX, barStartY + barWidth);
        glVertex2f(endX,                         barStartY + barWidth);
        glVertex2f(endX,                         barStartY);
    } glEnd();
}


}
