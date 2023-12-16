export module Measures.DataSources.IMusicDataSource;

import std.core;

namespace rg {

export struct MusicData {
    bool isMusicPlayerRunning{ false };
    bool isMusicPlaying{ false };
    std::string trackName;
    std::string artist;
    std::string album;
    int elapsedTime{ 0 };
    int totalTime{ 0 };

    auto operator<=>(const MusicData&) const = default;
};

export class IMusicDataSource {
public:
    virtual ~IMusicDataSource() = default;

    virtual MusicData getMusicData() const = 0;
};

}
