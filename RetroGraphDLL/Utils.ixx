export module Utils;

import Units;

import std.core;

import "WindowsHeaderUnit.h";

namespace rg {

export {

    /* Gets the directory of the executable file of this program */
    const std::string getExePath();

    /* Expands environment variables in path */
    const std::string getExpandedEnvPath(const std::string& path);

    /* Converts a wchar_t string to regular char string */
    std::string wstrToStr(const std::wstring& wstr);

    /* Converts a char string to wchar_t string */
    std::wstring strToWstr(const std::string& str);

    /* Converts FILETIME to an unsigned LARGE_INTEGER struct */
    ULARGE_INTEGER ftToULI(const FILETIME& ft);

    /* Subtracts the FILETIMES and returns result as 64 bit unsigned integer */
    uint64_t subtractTimes(const FILETIME& ftA, const FILETIME& ftB);

    template<std::integral T>
    T strToNum(std::string_view str) {
        T val;
        const auto ret{ std::from_chars(str.data(), str.data() + str.size(), val) };

        if (ret.ec == std::errc::invalid_argument)
            throw std::invalid_argument("strToNum failed");
        else if (ret.ec == std::errc::result_out_of_range)
            throw std::out_of_range("strToNum failed");

        return val;
    }

}

} // namespace rg
