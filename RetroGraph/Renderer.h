#pragma once

namespace rg {

class CPUMeasure;
class GPUMeasure;
class RAMMeasure;
class ProcessMeasure;
class DriveMeasure;
class SystemInfo;
class GLShaderHandler;

/* In charge of rendering each component's data to the window */
class Renderer {
public:
    Renderer(const CPUMeasure& _cpu, const GPUMeasure& _gpu, const RAMMeasure& _ram,
             const ProcessMeasure& _proc, const DriveMeasure& _drive, const SystemInfo& _sys);
    ~Renderer();

    void draw(const GLShaderHandler& shaders) const;
private:

     const CPUMeasure& m_cpuMeasure;
     const GPUMeasure& m_gpuMeasure;
     const RAMMeasure& m_ramMeasure;
     const ProcessMeasure& m_processMeasure;
     const DriveMeasure& m_driveMeasure;
     const SystemInfo& m_sysInfo;
};

}