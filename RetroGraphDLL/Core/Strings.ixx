export module RG.Core:Strings;

import std.core;

namespace rg {

/* Converts a wchar_t string to regular char string */
export std::string wstrToStr(const std::wstring& wstr);

/* Converts a char string to wchar_t string */
export std::wstring strToWstr(const std::string& str);

export template<std::integral T>
T strToNum(std::string_view str) {
    T val;
    const auto ret{ std::from_chars(str.data(), str.data() + str.size(), val) };

    if (ret.ec == std::errc::invalid_argument)
        throw std::invalid_argument("strToNum failed");
    else if (ret.ec == std::errc::result_out_of_range)
        throw std::out_of_range("strToNum failed");

    return val;
}

} // namespace rg
