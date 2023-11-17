export module Rendering.GLListContainer;

import Colors;
import Units;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export class GLListContainer {
public:
    static GLListContainer& inst() { static GLListContainer i; return i; }

    ~GLListContainer();

    void drawCircle() const { glCallList(m_circleList); }
    void drawViewportBorder() const { glCallList(m_vpBorderList); }
    void drawBorder() const { glCallList(m_borderList); }
    void drawWidgetBackground() const { glCallList(m_widgetBGList); }
    void drawTopAndBottomSerifs() const { glCallList(m_serifList); }
private:
    GLListContainer();

    void initCircleList() const;
    void initViewportBorderList() const;
    void initBorderList() const;
    void initSerifList() const;
    void initWidgetBGList() const;

    GLuint m_listStart;

    GLuint m_vpBorderList;
    GLuint m_borderList;
    GLuint m_serifList;
    GLuint m_circleList;
    GLuint m_widgetBGList;
};
GLListContainer::GLListContainer()
    : m_circleList{ glGenLists(1) }
    , m_vpBorderList{ glGenLists(1) }
    , m_borderList{ glGenLists(1) }
    , m_widgetBGList{ glGenLists(1) }
    , m_serifList{ glGenLists(1) } {

    initCircleList();
    initViewportBorderList();
    initBorderList();
    initSerifList();
    initWidgetBGList();

    GLenum error = glGetError();
    std::cout << error << '\n';
}


GLListContainer::~GLListContainer() {
    glDeleteLists(m_circleList, 1);
    glDeleteLists(m_borderList, 1);
    glDeleteLists(m_vpBorderList, 1);
    glDeleteLists(m_widgetBGList, 1);
    glDeleteLists(m_serifList, 1);
}

void GLListContainer::initCircleList() const {
    glNewList(m_circleList, GL_COMPILE); {
        glColor4f(PARTICLE_R, PARTICLE_G, PARTICLE_B, PARTICLE_A);
        glBegin(GL_TRIANGLE_FAN); {
            glVertex2f(0.0f, 0.0f);
            for (int i = 0; i < circleLines; ++i) {
                const auto theta{ 2.0f * 3.1415926f * static_cast<float>(i) /
                    static_cast<float>(circleLines - 1) };
                glVertex2f(cosf(theta), sinf(theta));
            }
        } glEnd();
    } glEndList();
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

}
