export module Units;

import <cstdint>;
//import std.core;

namespace rg {

export {

    constexpr auto circleLines = int{ 10 };
    constexpr auto serifLen = float{ 0.05f };

    constexpr uint64_t KB{ 1024 };
    constexpr uint64_t MB{ KB * 1024 };
    constexpr uint64_t GB{ MB * 1024 };

    constexpr int maxTicks{ 100000U };
    constexpr int tickDuration{ 10U }; // tick length in milliseconds
    constexpr int ticksPerSecond{ 1000U / tickDuration };

    constexpr uint64_t bToGB(uint64_t b) { return b / GB;  }
    constexpr uint64_t bToMB(uint64_t b) { return b / MB;  }
    constexpr uint64_t bToKB(uint64_t b) { return b / KB;  }

}

}