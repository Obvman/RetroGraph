#include "stdafx.h"
#include "DisplayMeasure.h"

#include "utils.h"

#include "FontManager.h"
#include "RetroGraph.h"

#include <iostream>

namespace rg {

DisplayMeasure::DisplayMeasure()
    : m_monitors{} {

    force_update();
}

}
