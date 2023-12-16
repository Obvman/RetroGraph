export module Core.Units;

import std.core;

namespace rg {

export constexpr int64_t KB{ 1024 };
export constexpr int64_t MB{ KB * 1024 };
export constexpr int64_t GB{ MB * 1024 };

export constexpr int64_t bToGB(int64_t b) { return b / GB;  }
export constexpr int64_t bToMB(int64_t b) { return b / MB;  }
export constexpr int64_t bToKB(int64_t b) { return b / KB;  }

}