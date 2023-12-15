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

    /* Converts FILETIME to an unsigned LARGE_INTEGER struct */
    ULARGE_INTEGER ftToULI(const FILETIME& ft);

    /* Subtracts the FILETIMES and returns result as 64 bit unsigned integer */
    uint64_t subtractTimes(const FILETIME& ftA, const FILETIME& ftB);
}

} // namespace rg
