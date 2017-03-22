#pragma once

#include <stdint.h>
#include <Windows.h>

#include "Measure.h"
#include "drawUtils.h"

namespace rg {

class ProcessMeasure;

/* Monitors current music player status. Is dependent on ProcessMeasure, so must
 * be created after ProcessMeasure is created
 */
class MusicMeasure : public Measure {
public:
    MusicMeasure(const ProcessMeasure* procMeasure);
    virtual ~MusicMeasure();
    MusicMeasure(const MusicMeasure&) = delete;
    MusicMeasure& operator=(const MusicMeasure&) = delete;

    virtual void init() override;
    virtual void update(uint32_t ticks) override;

    bool isPlayerRunning() const { return m_playerRunning; }
private:
    const ProcessMeasure* m_processMeasure;

    bool m_playerRunning;
    HWND m_playerHandle;
};
}
