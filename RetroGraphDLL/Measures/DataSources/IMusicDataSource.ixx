export module RG.Measures.DataSources:IMusicDataSource;

import std.core;

namespace rg {

export struct MusicData {
    bool isMusicPlayerRunning{ false };
    bool isMusicPlaying{ false };
    std::string trackName;
    std::string artist;
    std::string album;
    std::chrono::seconds elapsedTime{ 0 };
    std::chrono::seconds totalTime{ 0 };

    auto operator<=>(const MusicData&) const = default;
};

export class IMusicDataSource {
public:
    virtual ~IMusicDataSource() = default;

    virtual MusicData getMusicData() const = 0;
};

} // namespace rg
