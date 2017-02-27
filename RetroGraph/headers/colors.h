#pragma once

namespace rg {

constexpr float BLACK_R{ 0x00 / 255.0f };
constexpr float BLACK_G{ 0x00 / 255.0f };
constexpr float BLACK_B{ 0x00 / 255.0f };

constexpr float WHITE_R{ 0xFF / 255.0f };
constexpr float WHITE_G{ 0xFF / 255.0f };
constexpr float WHITE_B{ 0xFF / 255.0f };

constexpr float GREY_R{ 0x22 / 255.0f };
constexpr float GREY_G{ 0x22 / 255.0f };
constexpr float GREY_B{ 0x22 / 255.0f };

constexpr float BLUE1_R{ 0x01 / 255.0f };
constexpr float BLUE1_G{ 0xB9 / 255.0f };
constexpr float BLUE1_B{ 0xD2 / 255.0f };

constexpr float TURQUOISE1_R{ 0x43 / 255.0f };
constexpr float TURQUOISE1_G{ 0xB7 / 255.0f };
constexpr float TURQUOISE1_B{ 0xAF / 255.0f };

constexpr float PINK1_R{ 0xFF / 255.0f };
constexpr float PINK1_G{ 0x02 / 255.0f };
constexpr float PINK1_B{ 0xFF / 255.0f };

constexpr float PINK2_R{ 0xFF / 255.0f };
constexpr float PINK2_G{ 0x00 / 255.0f };
constexpr float PINK2_B{ 0xD6 / 255.0f };

constexpr float PINK3_R{ 0x77 / 255.0f };
constexpr float PINK3_G{ 0x00 / 255.0f };
constexpr float PINK3_B{ 0x43 / 255.0f };



constexpr float TEXT_R{ WHITE_R };
constexpr float TEXT_G{ WHITE_G };
constexpr float TEXT_B{ WHITE_B };
constexpr float TEXT_A{ 0.7f };

constexpr float DEBUG_BORDER_R{ 0.2f };
constexpr float DEBUG_BORDER_G{ 0.8f };
constexpr float DEBUG_BORDER_B{ 0.2f };

constexpr float LINE_R{ WHITE_R };
constexpr float LINE_G{ WHITE_G };
constexpr float LINE_B{ WHITE_B };

constexpr float DIVIDER_R{ WHITE_R };
constexpr float DIVIDER_G{ WHITE_G };
constexpr float DIVIDER_B{ WHITE_B };

constexpr float BGCOLOR_R{ BLACK_R };
constexpr float BGCOLOR_G{ BLACK_G };
constexpr float BGCOLOR_B{ BLACK_B };
constexpr float BGCOLOR_A{ 0.0f };


constexpr float BARFILLED_R{ WHITE_R };
constexpr float BARFILLED_G{ WHITE_G };
constexpr float BARFILLED_B{ WHITE_B };

constexpr float BARFREE_R{ GREY_R };
constexpr float BARFREE_G{ GREY_G };
constexpr float BARFREE_B{ GREY_B };
}