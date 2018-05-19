#pragma once

#include "stdafx.h"

#include <cstdint>

namespace rg {

constexpr uint64_t KB{ 1024 };
constexpr uint64_t MB{ KB * 1024 };
constexpr uint64_t GB{ MB * 1024 };

constexpr uint32_t maxTicks{ 100000U };
constexpr uint32_t tickDuration{ 40U }; // tick length in milliseconds
constexpr uint32_t ticksPerSecond{ 1000U / tickDuration };

constexpr uint64_t bToGB(uint64_t b) { return b / GB;  }
constexpr uint64_t bToMB(uint64_t b) { return b / MB;  }
constexpr uint64_t bToKB(uint64_t b) { return b / KB;  }

} // namespace rg