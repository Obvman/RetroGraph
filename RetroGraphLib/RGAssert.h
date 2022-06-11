#pragma once

// Debug Assertions
#ifdef _DEBUG

constexpr auto debugMode = bool{ true };

#define RGASSERT(expr, str) rg::rgAssert(expr, str)
#define RGERROR(str) rg::rgError(str)

#else

constexpr auto debugMode = bool{ false };

#define RGASSERT(expr, str)
#define RGERROR(str)

#endif

// Throws error if expr is false. Always evaluates expr - even in release mode
#define RGVERIFY(expr, str) rgVerify(expr, str)

namespace rg {

/* Displays an Error message box with the given string as a message */
void showMessageBox(const char* s);

/* Displays an Error message box with the given string as a message and
   exits the program with a failure code */
void fatalMessageBox(const char* s);

// Throws error if expr is false
void rgAssert(bool expr, const char* str);

// Throws error
void rgError(const char* str);

void rgVerify(bool expr [[maybe_unused]], const char* str [[maybe_unused]] );

#ifdef _DEBUG
constexpr auto debugMode = bool{ true };
#else
constexpr auto debugMode = bool{ false };
#endif

}
