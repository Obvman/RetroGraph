export module RG.Rendering:VBO;

import :DrawUtils;

import std.core;

import "GLHeaderUnit.h";
import "RGAssert.h";

namespace rg {

export class [[nodiscard]] VBOBindScope {
public:
    VBOBindScope(GLuint id, GLenum target_)
        : target{ target_ } {
        glBindBuffer(target, id);
    }
    ~VBOBindScope() { glBindBuffer(target, 0); }

    VBOBindScope(const VBOBindScope&) = delete;
    VBOBindScope& operator=(const VBOBindScope&) = delete;
    VBOBindScope(VBOBindScope&&) = delete;
    VBOBindScope& operator=(VBOBindScope&&) = delete;

private:
    GLenum target;
};

// Container for standard VBO data (no element array)
export class VBO {
public:
    // Default constructed that does not allocate any buffer
    VBO() noexcept
        : id{ invalidGLID }
        , target{ GL_ARRAY_BUFFER }
        , usage{ GL_STATIC_DRAW } {}

    explicit VBO(GLenum target_, GLenum usage_) noexcept;

    virtual ~VBO();

    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;

    VBO(VBO&& other);
    VBO& operator=(VBO&& other);

    VBOBindScope bind() const { return { id, target }; }

    void bufferData(GLsizeiptr bytes, const void* data) const { glBufferData(target, bytes, data, usage); }

    void bufferSubData(GLintptr offset, GLsizeiptr bytes, const void* data) const {
        glBufferSubData(target, offset, bytes, data);
    }

private:
    GLuint id;
    GLenum target;
    GLenum usage;
};

// A VBO that owns the buffer itself
export template<class T>
class OwningVBO : public VBO {
public:
    OwningVBO(GLsizei size, GLenum target, GLenum usage) noexcept
        : VBO{ target, usage }
        , m_data(size) {}

    OwningVBO(OwningVBO&& other) { *this = std::move(other); }
    OwningVBO& operator=(OwningVBO&& other) {
        if (this != &other) {
            VBO::operator=(std::move(other));
            m_data = std::move(other.m_data);
        }

        return *this;
    }

    std::vector<T>& data() { return m_data; }
    const std::vector<T>& data() const { return m_data; }
    GLsizei size() const { return static_cast<GLsizei>(m_data.size()); }
    GLsizei sizeBytes() const { return size() * elemBytes(); }
    GLsizei elemBytes() const { return sizeof(T); }

    void bufferData() const { VBO::bufferData(sizeBytes(), m_data.data()); }

    void bufferSubData(GLintptr offset, GLsizeiptr bytes) const {
        RGASSERT(offset + bytes <= sizeBytes(), "bufferSubData call out of range");
        VBO::bufferSubData(offset, bytes, m_data.data());
    }

private:
    std::vector<T> m_data;
};

} // namespace rg
