module;

#include "RGAssert.h"

export module VBOController;

import AnimationState;
import Colors;
import DrawUtils;
import GLShader;
import UserSettings;

import <concepts>;
import <utility>;
import <variant>;
import <vector>;

import <GLHeaders.h>;

namespace rg {

export class VBOController;
export class VBOID;

// Automatically binds/unbinds given VBOs and executes the function given
void vboElemArrayDrawScope(GLuint vertID, GLuint indexID, std::regular_invocable auto f) {
    glBindBuffer(GL_ARRAY_BUFFER, vertID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    f();

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void vboDrawScope(GLuint vertID, std::regular_invocable auto f) {
    glBindBuffer(GL_ARRAY_BUFFER, vertID);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    f();

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Container for standard VBO data (no element array)
struct VBOContainer {
    VBOContainer() noexcept : VBOContainer{ 0 } { }

    VBOContainer(GLsizei size_) noexcept
        : id{ 0 }
        , size{ size_ }
        , data( size_ * 2)  { }

    GLuint id;
    GLsizei size; // Num vertices
    std::vector<GLfloat> data;
};


class VBOID;

class VBOController {
public:
    static VBOController & inst() { static VBOController i; return i; }
    ~VBOController();

    VBOID createGraphLineVBO(size_t numValues);
    VBOID createAnimationVBO(size_t numLines);

    // Assumes values are floating percentages between 0 and 100
    void updateGraphLines(const VBOID& vboID, const std::vector<GLfloat>& values);
    void updateAnimationVBO(const VBOID& vboID, const AnimationState& as);

    void drawGraphGrid() const;
    void drawGraphLines(const VBOID& vboID) const;
    void drawAnimationVBO(const VBOID & vboID, int size) const;
private:
    VBOController();

    void initVBOs();
    void initGraphGridVBO();

    void destroyVBO(const VBOID& vboIdx);

    GLuint m_graphGridVertsID;
    GLuint m_graphGridIndicesID;
    GLsizei m_graphGridIndicesSize;

    GLShader m_particleLineShader;

    std::vector<VBOContainer> m_graphLineVBOData;

    GLuint m_animColors;

    friend class VBOID;
};

// Used to index VBOController::m_graphLineVBOData
// not to be confused with id member of VBOContainer, which contains the openGL
// provided id of the VBO
class VBOID {
public:
    VBOID() noexcept : m_id{ -1 } {}
    explicit VBOID(int id) noexcept : m_id{ id } {}
    explicit VBOID(size_t id) noexcept : m_id{ static_cast<int>(id) } {}
    ~VBOID() {
        VBOController::inst().destroyVBO(*this);
    }

    // Very important that we never copy!
    VBOID(const VBOID&) = delete;
    VBOID& operator=(const VBOID&) = delete;
    VBOID(VBOID&&) noexcept;
    VBOID& operator=(VBOID&&) noexcept;

    // Conversion to int
    explicit operator int() const { return m_id; }
    operator bool() const { return m_id != -1; }

private:
    int m_id;
};


VBOController::VBOController()
    : m_graphLineVBOData{}
    , m_particleLineShader{ "test" }
    , m_graphGridIndicesID{ 0 } {

    //RGASSERT(m_particleLineShader, "Failed to init shader\n");

    initVBOs();
}


VBOController::~VBOController() {
    glDeleteBuffers(1, &m_graphGridVertsID);
    glDeleteVertexArrays(1, &m_graphGridIndicesID);

    for (const auto& vbo : m_graphLineVBOData)
        if (vbo.id != UINT_MAX)
            glDeleteBuffers(1, &vbo.id);
}

void VBOController::initVBOs() {
    initGraphGridVBO();
}

void VBOController::initGraphGridVBO() {
    constexpr auto numVertLines = size_t{ 14U };
    constexpr auto numHorizLines = size_t{ 7U };

    auto gVerts = std::vector<GLfloat>{};
    auto gIndices = std::vector<GLuint>{};
    gVerts.reserve(4 * (numVertLines + numHorizLines));
    gIndices.reserve(2 * (numVertLines + numHorizLines));

    // Fill the vertex and index arrays with data for drawing grid as VBO
    for (unsigned int i = 0U; i < numVertLines; ++i) {
        const float x{ (i) / static_cast<float>(numVertLines - 1) * 2.0f - 1.0f };
        gVerts.push_back(x);
        gVerts.push_back(1.0f); // Vertical line top vert

        gVerts.push_back(x);
        gVerts.push_back(-1.0f); // Vertical line bottom vert

        gIndices.push_back(2 * i);
        gIndices.push_back(2 * i + 1);
    }

    // Offset value for the index array
    const auto vertLineIndexCount{ static_cast<unsigned int>(gIndices.size()) };
    for (unsigned int i = 0U; i < numHorizLines; ++i) {
        const float y{ static_cast<float>(i) / (numHorizLines - 1) * 2.0f - 1.0f };
        gVerts.push_back(-1.0f);
        gVerts.push_back(y); // Horizontal line left vert

        gVerts.push_back(1.0f);
        gVerts.push_back(y); // Horizontal line right vert

        gIndices.push_back(vertLineIndexCount + 2 * i);
        gIndices.push_back(vertLineIndexCount + 2 * i + 1);
    }
    m_graphGridIndicesSize = static_cast<GLsizei>(gIndices.size());


    // Initialise the graph grid VBO
    glGenBuffers(1, &m_graphGridVertsID);
    glBindBuffer(GL_ARRAY_BUFFER, m_graphGridVertsID);
    glBufferData(GL_ARRAY_BUFFER, gVerts.size() * sizeof(GLfloat),
                 gVerts.data(), GL_STATIC_DRAW);

    // Initialise graph grid index array
    glGenBuffers(1, &m_graphGridIndicesID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_graphGridIndicesID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gIndices.size() * sizeof(GLuint),
                 gIndices.data(), GL_STATIC_DRAW);

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VBOController::drawGraphGrid() const {
    vboElemArrayDrawScope(m_graphGridVertsID, m_graphGridIndicesID, [this]() {
        glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.2f);
        glLineWidth(0.5f);
        glDrawElements(GL_LINES, m_graphGridIndicesSize, GL_UNSIGNED_INT, nullptr);
    });
}

void VBOController::drawGraphLines(const VBOID& vboID) const {
    vboDrawScope(m_graphLineVBOData[static_cast<int>(vboID)].id, [this, &vboID]() {
        glDrawArrays(GL_LINE_STRIP, 0, m_graphLineVBOData[static_cast<int>(vboID)].size);
    });
}

void VBOController::drawAnimationVBO(const VBOID & vboID, int size) const {
    // TODO colors!
    glBindBuffer(GL_ARRAY_BUFFER, m_graphLineVBOData[static_cast<int>(vboID)].id);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    //glBindBuffer(GL_ARRAY_BUFFER, m_animColors);
    //glVertexAttribPointer(1U, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    //glEnableVertexAttribArray(1U);

    //glUseProgram(particleLineShader);

    //glColor4f(PARTICLE_R, PARTICLE_G, PARTICLE_B, 1.0f);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(size));

    //glUseProgram(0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VBOID VBOController::createGraphLineVBO(size_t numValues) {
    auto& vboContainer{ m_graphLineVBOData.emplace_back(static_cast<GLsizei>(numValues)) };
    auto& verts{ vboContainer.data };
    std::vector<GLfloat> values(numValues, 0.0f);

    for (size_t i = 0; i < values.size(); ++i) {
        verts[2 * i] = (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f;
        verts[2 * i + 1] = values[i] * 2.0f - 1.0f;
    }

    glGenBuffers(1, &vboContainer.id);
    glBindBuffer(GL_ARRAY_BUFFER, vboContainer.id);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_DYNAMIC_DRAW);

    return VBOID{ m_graphLineVBOData.size() - 1 };
}

VBOID VBOController::createAnimationVBO(size_t numLines) {
    // Each line has 2 vertices...
    auto& vboContainer{ m_graphLineVBOData.emplace_back(static_cast<GLsizei>(numLines * 2)) };
    auto& verts{ vboContainer.data };

    // TODO create color buffer

    glGenBuffers(1, &vboContainer.id);
    glBindBuffer(GL_ARRAY_BUFFER, vboContainer.id);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_DYNAMIC_DRAW);

    //std::vector<GLfloat> colors(verts.size() * 2, 1.0f); // 4 color components vs 2 coords = double size
    //glGenBuffers(1, &m_animColors);
    //glBindBuffer(GL_ARRAY_BUFFER, m_animColors);
    //glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), GL_DYNAMIC_DRAW);

    return VBOID{ m_graphLineVBOData.size() - 1 };
}

void VBOController::destroyVBO(const VBOID& vboIdx) {
    if (!vboIdx)
        return;

    auto& container{ m_graphLineVBOData[static_cast<int>(vboIdx)] };

    if (container.id != UINT_MAX)
        glDeleteBuffers(1, &container.id);

    // We can't shift the elements of the container around, since outside
    // resources point to indexes. So we 'destroy' it by copying to an empty container
    container = VBOContainer{}; // Clear resources
    container.id = UINT_MAX;
}

void VBOController::updateGraphLines(const VBOID& vboID, const std::vector<GLfloat>& values) {
    auto& vboContainer{ m_graphLineVBOData[static_cast<int>(vboID)] };
    auto& verts{ vboContainer.data };

    // Value vectors can change size. In this case we only to allocate buffer data instead of just writing to it
    if (vboContainer.size != values.size()) {
        vboContainer.size = static_cast<GLsizei>(values.size());
        verts.resize(vboContainer.size * 2);

        for (size_t i = 0; i < values.size(); ++i) {
            verts[2 * i] = (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f;
            verts[2 * i + 1] = values[i] * 2.0f - 1.0f;
        }

        glBindBuffer(GL_ARRAY_BUFFER, vboContainer.id);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_DYNAMIC_DRAW);
    } else {
        for (size_t i = 0; i < values.size(); ++i) {
            verts[2 * i] = (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f;
            verts[2 * i + 1] = values[i] * 2.0f - 1.0f;
        }

        glBindBuffer(GL_ARRAY_BUFFER, vboContainer.id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(GLfloat), verts.data());
    }
}

void VBOController::updateAnimationVBO(const VBOID& vboID, const AnimationState & as) {
    auto& vboContainer{ m_graphLineVBOData[static_cast<int>(vboID)] };
    auto& verts{ vboContainer.data };

    const auto numLines{ as.getNumLines() };

    for (int i = 0; i < numLines; ++i) {
        const auto& line{ as.getLines()[i] };
        verts[4 * i]     = line.x1;
        verts[4 * i + 1] = line.y1;
        verts[4 * i + 2] = line.x2;
        verts[4 * i + 3] = line.y2;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vboContainer.id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numLines * sizeof(ParticleLine), verts.data());
}


VBOID::VBOID(VBOID&& other) noexcept {
    m_id = other.m_id;
    other.m_id = -1;
}

VBOID& VBOID::operator=(VBOID&& other) noexcept {
    if (&other != this) {
        m_id = other.m_id;
        other.m_id = -1;
    }
    return *this;
}

}
