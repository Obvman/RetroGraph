export module RG.Core:Profiling;

import std.core;

namespace rg {

using namespace std::chrono;

high_resolution_clock::duration recordTimeToExecute(std::regular_invocable auto f) {
    const auto start{ std::chrono::high_resolution_clock::now() };

    f();

    const auto end{ std::chrono::high_resolution_clock::now() };
    return end - start;
}

/* Prints how long the given function f took to execute */
export void printTimeToExecuteMs(const char* funcName, std::regular_invocable auto f) {
    const microseconds executionTime{ duration_cast<microseconds>(recordTimeToExecute(f)) };

    printf("%s took %f milliseconds\n", funcName, static_cast<double>(executionTime.count()) / 1000.0);
}

export void printTimeToExecuteMs(std::regular_invocable auto f) {
    printTimeToExecuteMs("Function", f);
}

export void printTimeToExecuteTicks(const char* funcName, std::regular_invocable auto f) {
    const auto executionTime{ recordTimeToExecute(f) };
    printf("%s took %I64d ticks\n", funcName, executionTime.count());
}

export void printTimeToExecuteTicks(std::regular_invocable auto f) {
    printTimeToExecuteTicks("Function", f);
}

} // namespace rg
