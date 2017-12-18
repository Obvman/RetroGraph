#pragma once

#include <stdint.h>

namespace rg {

constexpr uint64_t KB{ 1024 };
constexpr uint64_t MB{ KB * 1024 };
constexpr uint64_t GB{ MB * 1024 };

constexpr uint32_t maxTicks{ 100000U };
constexpr uint32_t tickDuration{ 100U }; // tick length in milliseconds
constexpr uint32_t ticksPerSecond{ 1000U / tickDuration };

}