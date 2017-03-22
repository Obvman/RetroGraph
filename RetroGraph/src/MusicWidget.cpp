#include "../headers/MusicWidget.h"

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
        m_fontManager->renderLine(RG_FONT_TIME, "FOOBAR", 0, 0, 0, 0,
                RG_ALIGN_TOP | RG_ALIGN_LEFT, 10, 10);
    }
}


}
