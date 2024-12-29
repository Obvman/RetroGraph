module RG.Core:Strings;

import std;

// TODO does std have wide string conversions?
import "WindowsHeaderUnit.h";

namespace rg {

std::string wstrToStr(const std::wstring& wstr) {
    if (wstr.empty())
        return {};

    const auto size_needed{ WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), nullptr, 0,
                                                nullptr, nullptr) };

    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), &strTo[0], size_needed, nullptr, nullptr);
    return strTo;
}

std::wstring strToWstr(const std::string& str) {
    if (str.empty())
        return {};

    const auto size_needed{ MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), nullptr, 0) };
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string ltrim(const std::string& str) {
    std::string cpy{ str };
    cpy.erase(cpy.cbegin(), std::find_if(cpy.cbegin(), cpy.cend(), [](unsigned char ch) { return !std::isspace(ch); }));
    return cpy;
}

std::string rtrim(const std::string& str) {
    std::string cpy{ str };
    cpy.erase(std::find_if(cpy.crbegin(), cpy.crend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
              cpy.cend());
    return cpy;
}

std::string trim(const std::string& str) {
    return rtrim(ltrim(str));
}

} // namespace rg
