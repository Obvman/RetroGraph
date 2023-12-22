module RG.Measures:MusicMeasure;

namespace rg {

MusicMeasure::MusicMeasure(std::chrono::milliseconds updateInterval,
                           std::unique_ptr<const IMusicDataSource> musicDataSource)
    : Measure{ updateInterval }
    , m_musicDataSource{ std::move(musicDataSource) }
    , m_musicData{ m_musicDataSource->getMusicData() } {}

bool MusicMeasure::updateInternal() {
    const MusicData oldMusicData{ m_musicData };
    m_musicData = m_musicDataSource->getMusicData();
    return oldMusicData != m_musicData;
}

} // namespace rg
