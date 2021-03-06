#include "stdafx.h"
#include "DisplayMeasure.h"

#include "utils.h"

#include "FontManager.h"
#include "RetroGraph.h"

#include <iostream>

namespace rg {

DisplayMeasure::DisplayMeasure()
    : Measure{ 2U }
    , m_monitors{} {

    force_update();
}

void DisplayMeasure::update(int ticks) {
    (void)ticks;
}

bool rg::DisplayMeasure::shouldUpdate(int ticks) const {
    return ticksMatchSeconds(ticks, m_updateRates.front());
}

}
