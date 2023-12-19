module RG.Measures:DisplayMeasure;

namespace rg {

DisplayMeasure::DisplayMeasure()
    : Measure{ std::nullopt }
    , m_monitors{} {

    updateInternal();
}

} // namespace rg
