export module Units;

import std.core;

namespace rg {

export {

    constexpr int64_t KB{ 1024 };
    constexpr int64_t MB{ KB * 1024 };
    constexpr int64_t GB{ MB * 1024 };

    constexpr int maxTicks{ 100000U };
    constexpr int tickDuration{ 10U }; // tick length in milliseconds
    constexpr int ticksPerSecond{ 1000U / tickDuration };

    constexpr int64_t bToGB(int64_t b) { return b / GB;  }
    constexpr int64_t bToMB(int64_t b) { return b / MB;  }
    constexpr int64_t bToKB(int64_t b) { return b / KB;  }

}

}