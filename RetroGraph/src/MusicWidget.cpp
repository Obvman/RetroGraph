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

        glViewport(m_viewport.x, m_viewport.y,
                   m_viewport.width, m_viewport.height/4);
        drawHorizontalProgressBar(0.3f, -0.8f, 0.8f, 
                static_cast<float>(m_musicMeasure->getElapsedTime()), 
                static_cast<float>(m_musicMeasure->getTotalTime()));
    } else {
        m_fontManager->renderLine(RG_FONT_TIME, "No Media", 0, 0, 0, 0,
                RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_CENTERED_HORIZONTAL, 10, 10);
    }
}

}
