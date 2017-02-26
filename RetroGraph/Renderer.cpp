#include "Renderer.h"

#include "CPUMeasure.h"
#include "GPUMeasure.h"
#include "RAMMeasure.h"
#include "ProcessMeasure.h"
#include "DriveMeasure.h"
#include "SystemInfo.h"
#include "GLShaderHandler.h"
#include "colors.h"

namespace rg {

Renderer::Renderer(const CPUMeasure& _cpu, const GPUMeasure& _gpu, const RAMMeasure& _ram,
                   const ProcessMeasure& _proc, const DriveMeasure& _drive, const SystemInfo& _sys) :
    m_cpuMeasure{ _cpu },
    m_gpuMeasure{ _gpu },
    m_ramMeasure{ _ram },
    m_processMeasure{ _proc },
    m_driveMeasure{ _drive },
    m_sysInfo{ _sys } {

}


Renderer::~Renderer() {
}

void Renderer::draw(const GLShaderHandler& shaders) const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

    m_cpuMeasure.draw(shaders.getCpuGraphProgram());
    //m_gpuMeasure.draw();
    m_ramMeasure.draw();
    m_processMeasure.draw();
    m_driveMeasure.draw();
    m_sysInfo.draw();
}

}