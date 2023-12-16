module RG.Measures:MusicMeasure;

namespace rg {

MusicMeasure::MusicMeasure(std::unique_ptr<const IMusicDataSource> musicDataSource)
    : m_musicDataSource{ std::move(musicDataSource) } {

    updateInternal();
}

bool MusicMeasure::updateInternal() {
    const MusicData oldMusicData{ m_musicData };
    m_musicData = m_musicDataSource->getMusicData();
    return oldMusicData != m_musicData;
}

} // namespace rg
