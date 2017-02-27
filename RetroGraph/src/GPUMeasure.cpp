#include "../headers/GPUMeasure.h"

namespace rg {

constexpr GLint GL_GPU_MEM_TOTAL{ 0x9048 };
constexpr GLint GL_GPU_MEM_CURRENT_AVAIL { 0x9049 };

GPUMeasure::GPUMeasure() :
    m_totalGPUMemoryKB{ 0 },
    m_currAvailableGPUMemoryKB{ 0 } {

    glGetIntegerv(GL_GPU_MEM_TOTAL, &m_totalGPUMemoryKB);
    glGetIntegerv(GL_GPU_MEM_CURRENT_AVAIL, &m_currAvailableGPUMemoryKB);
}


GPUMeasure::~GPUMeasure() {
}

void GPUMeasure::update() {
    glGetIntegerv(GL_GPU_MEM_TOTAL, &m_totalGPUMemoryKB);
    glGetIntegerv(GL_GPU_MEM_CURRENT_AVAIL, &m_currAvailableGPUMemoryKB);
}

}