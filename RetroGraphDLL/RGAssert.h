#pragma once

// Debug Assertions
#ifdef _DEBUG

inline constexpr auto debugMode = bool{ true };

#define RGASSERT(expr, str) rg::rgAssert(expr, str)
#define RGERROR(str) rg::rgError(str)

#else

inline constexpr auto debugMode = bool{ false };

#define RGASSERT(expr, str) ((void)(expr)); ((void)(str))
#define RGERROR(str) ((void)(str))

#endif

// Throws error if expr is false. Always evaluates expr - even in release mode
#define RGVERIFY(expr, str) rgVerify(expr, str)

namespace rg {

/* Displays an Error message box with the given string as a message */
void showMessageBox(const char* s);

// Throws error if expr is false
__declspec(dllexport) void rgAssert(bool expr, const char* str);

// Throws error
__declspec(dllexport) void rgError(const char* str);

__declspec(dllexport) void rgVerify([[maybe_unused]] bool expr, [[maybe_unused]] const char* str);

}
