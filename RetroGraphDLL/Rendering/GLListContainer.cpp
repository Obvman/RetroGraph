module Rendering.GLListContainer;

import Colors;

import "GLHeaderUnit.h";

namespace rg {

GLListContainer::GLListContainer()
    : m_vpBorderList{ glGenLists(1) }
    , m_borderList{ glGenLists(1) }
    , m_widgetBGList{ glGenLists(1) }
    , m_serifList{ glGenLists(1) } {

    initViewportBorderList();
    initBorderList();
    initSerifList();
    initWidgetBGList();
}

GLListContainer::~GLListContainer() {
    glDeleteLists(m_borderList, 1);
    glDeleteLists(m_vpBorderList, 1);
    glDeleteLists(m_widgetBGList, 1);
    glDeleteLists(m_serifList, 1);
}

void GLListContainer::initViewportBorderList() const {
    glNewList(m_vpBorderList, GL_COMPILE); {
        float color[4];
        glGetFloatv(GL_CURRENT_COLOR, color);

        glColor3f(DEBUG_BORDER_R, DEBUG_BORDER_G, DEBUG_BORDER_B);

        // Preserve initial line width
        GLfloat lineWidth;
        glGetFloatv(GL_LINE_WIDTH, &lineWidth);

        glLineWidth(5.0f);

        glBegin(GL_LINES); {
            glVertex2f(-1.0f, -1.0f);
            glVertex2f(-1.0f,  1.0f);

            glVertex2f(-1.0f,  1.0f);
            glVertex2f( 1.0f,  1.0f);

            glVertex2f( 1.0f,  1.0f);
            glVertex2f( 1.0f, -1.0f);

            glVertex2f( 1.0f, -1.0f);
            glVertex2f(-1.0f, -1.0f);
        } glEnd();

        glColor4f(color[0], color[1], color[2], color[3]);
        glLineWidth(lineWidth);
    } glEndList();
}

void GLListContainer::initBorderList() const {
    glNewList(m_borderList, GL_COMPILE); {
        glColor4f(BORDER_R, BORDER_G, BORDER_B, BORDER_A);
        glLineWidth(0.5f);
        glBegin(GL_LINE_STRIP); {
            glVertex2f(-1.0f, 1.0f);
            glVertex2f(1.0f, 1.0f);
            glVertex2f(1.0f, -1.0f);
            glVertex2f(-1.0f, -1.0f);
            glVertex2f(-1.0f, 1.0f);
        } glEnd();
    } glEndList();
}

void GLListContainer::initSerifList() const {
    glNewList(m_serifList, GL_COMPILE); {
        glColor4f(DIVIDER_R, DIVIDER_G, DIVIDER_B, DIVIDER_A);
        glLineWidth(0.5f);
        glBegin(GL_LINES); {
            glVertex2f(-1.0f, 1.0f);
            glVertex2f(1.0f, 1.0f);

            glVertex2f(-1.0f, 1.0f-serifLen);
            glVertex2f(-1.0f, 1.0f);

            glVertex2f(1.0f, 1.0f-serifLen);
            glVertex2f(1.0f, 1.0f);

            // Bottom
            glVertex2f(-1.0f, -1.0f);
            glVertex2f(1.0f, -1.0f);

            glVertex2f(-1.0f, -1.0f);
            glVertex2f(-1.0f, -1.0f + serifLen);

            glVertex2f(1.0f, -1.0f);
            glVertex2f(1.0f, -1.0f + serifLen);
        } glEnd();
    } glEndList();
}

void GLListContainer::initWidgetBGList() const {
    glNewList(m_widgetBGList, GL_COMPILE); {
        glColor4f(WBG_R, WBG_G, WBG_B, WBG_A);
        glBegin(GL_QUADS); {
            glVertex2f(-1.0f,  1.0f);
            glVertex2f( 1.0f,  1.0f);
            glVertex2f( 1.0f, -1.0f);
            glVertex2f(-1.0f, -1.0f);
        } glEnd();
    } glEndList();
}

} // namespace rg
