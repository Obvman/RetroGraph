#include "../headers/MainWidget.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <Windows.h>

#include "../headers/colors.h"
#include "../headers/FontManager.h"

namespace rg {

MainWidget::MainWidget() {

}

MainWidget::~MainWidget() {

}

void MainWidget::init(const FontManager* fontManager,
                      Viewport viewport) {

    m_fontManager = fontManager;
    m_viewport = viewport;
}

void MainWidget::draw() const {
    glViewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);

    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);
}

}
