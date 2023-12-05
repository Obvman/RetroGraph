export module Widgets.Graph.MirrorLineGraph;

import Rendering.VBO;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export class MirrorLineGraph {
public:
    MirrorLineGraph(size_t numTopGraphSamples, size_t numBottomGraphSamples);

    void updatePoints(const std::vector<float>& topValues, const std::vector<float>& bottomValues);
    void draw() const;

protected:
    virtual void updatePointsVBO(OwningVBO<glm::vec2>& vbo, const std::vector<float>& values);

    OwningVBO<glm::vec2> m_topGraphPointsVBO;
    OwningVBO<glm::vec2> m_bottomGraphPointsVBO;

private:
    void initPointsVBO(OwningVBO<glm::vec2>& vbo);

    void drawPoints(const OwningVBO<glm::vec2>& vbo) const;
};

constexpr auto defaultPrecisionPoints = size_t{ 300 };

export class SmoothMirrorLineGraph : public MirrorLineGraph {
public:
    SmoothMirrorLineGraph(size_t precisionPoints = defaultPrecisionPoints);

private:
    void updatePointsVBO(OwningVBO<glm::vec2>& vbo, const std::vector<float>& values) override;

    size_t m_precisionPoints;
};
}
