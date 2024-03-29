module RG.Rendering:DrawUtils;

import :GLListContainer;

import "GLHeaderUnit.h";

namespace rg {

void scissorClear(GLint x, GLint y, GLint w, GLint h) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, w, h);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}

void drawWidgetBackground() {
    GLListContainer::inst().drawWidgetBackground();
}

void drawVerticalProgressBar(float barWidth, float startY, float endY, float currValue, float totalValue,
                             bool warningColor) {
    const auto percentage{ currValue / totalValue };
    const auto startX = float{ ((viewportWidth - barWidth) / viewportWidth) - 1.0f };
    const auto rangeY{ endY - startY };

    glBegin(GL_QUADS);
    {
        glColor4f(BARFILLED_R, BARFILLED_G, BARFILLED_B, BARFILLED_A);
        glVertex2f(startX + barWidth, startY);
        glVertex2f(startX, startY);
        // Draw high percentages in warning colour gradient
        if (warningColor && percentage > 0.8f) {
            glColor4f(WARNING_BAR_R, WARNING_BAR_G, WARNING_BAR_B, 0.4f);
        }
        glVertex2f(startX, startY + percentage * rangeY);
        glVertex2f(startX + barWidth, startY + percentage * rangeY);

        glColor3f(BARFREE_R, BARFREE_G, BARFREE_B);
        glVertex2f(startX, startY + percentage * rangeY);
        glVertex2f(startX, endY);
        glVertex2f(startX + barWidth, endY);
        glVertex2f(startX + barWidth, startY + percentage * rangeY);
    }
    glEnd();
}

void drawHorizontalProgressBar(float barWidth, float startX, float endX, float currValue, float totalValue) {
    const auto percentage{ currValue / totalValue };
    const auto barStartY = float{ ((viewportWidth - barWidth) / viewportWidth) - 1.0f };
    const auto rangeX{ endX - startX };

    glBegin(GL_QUADS);
    {
        glColor4f(BARFILLED_R, BARFILLED_G, BARFILLED_B, BARFILLED_A);
        glVertex2f(startX, barStartY);
        glVertex2f(startX, barStartY + barWidth);
        glVertex2f(startX + percentage * rangeX, barStartY + barWidth);
        glVertex2f(startX + percentage * rangeX, barStartY);

        glColor3f(BARFREE_R, BARFREE_G, BARFREE_B);
        glVertex2f(startX + percentage * rangeX, barStartY);
        glVertex2f(startX + percentage * rangeX, barStartY + barWidth);
        glVertex2f(endX, barStartY + barWidth);
        glVertex2f(endX, barStartY);
    }
    glEnd();
}

void drawSerifLine(GLfloat x1, GLfloat x2, GLfloat y) {
    glBegin(GL_LINES);
    {
        glVertex2f(x1, y);
        glVertex2f(x2, y);

        glVertex2f(x1, y - serifLen);
        glVertex2f(x1, y + serifLen);

        glVertex2f(x2, y - serifLen);
        glVertex2f(x2, y + serifLen);
    }
    glEnd();
}

void setGLViewport(const Viewport& vp) {
    glViewport(vp.x, vp.y, vp.width, vp.height);
}

Viewport getGLViewport() {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    return { vp[0], vp[1], vp[2], vp[3] };
}

GLenum checkGLErrors() {
    GLenum err{ GL_NO_ERROR };
    GLenum lastErr{ GL_NO_ERROR };
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << err << std::endl;
        lastErr = err;
    }
    return lastErr;
}

} // namespace rg
