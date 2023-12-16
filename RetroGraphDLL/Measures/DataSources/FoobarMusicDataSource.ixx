export module RG.Measures.DataSources:FoobarMusicDataSource;

import :IMusicDataSource;

import std.core;

import "WindowsHeaderUnit.h";

namespace rg {

export class FoobarMusicDataSource : public IMusicDataSource {
public:
    MusicData getMusicData() const override;

private:
    std::string getFoobarWindowTitle(HWND playerHandle) const;

    // TODO can we use foobar SDK instead?
    // Populates music data using specifically formatted foobar window titles:
    // %title% | [%album artist%] | [%album%[ CD%discnumber%][ #%tracknumber%]] | $if(%ispaused%,Paused,Playing) | [%_time_elapsed_seconds%, %_time_total_seconds%] | 
    void populateDataFromTitle(const std::string& playerWindowTitle, MusicData& musicData) const;

    mutable std::string m_playerWindowTitleCache;
    mutable MusicData m_musicDataCache;
};

}
