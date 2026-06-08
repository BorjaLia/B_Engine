#pragma once

#include <string>
#include <cstdint>
#include <format>
#include <sstream>
#include <type_traits>
#include <string_view>

#pragma region Formatter Overload
// 1. Concept that detects operator<<, but IGNORES basic types (ints, strings)
template<typename T>
concept CustomStreamable = requires(std::ostream & os, const T & obj)
{
    os << obj;
} && !std::is_convertible_v<T, std::string_view> // Ignore strings
&& !std::is_arithmetic_v<T>;                     // Ignore ints, floats, bools

// 2. Clean specialization without inheritance for std::format
template <CustomStreamable T>
struct std::formatter<T>
{
    // MSVC requires this function to exist explicitly
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    // The const method with std::format_to is the C++20 standard
    template <typename FormatContext>
    auto format(const T& obj, FormatContext& ctx) const
    {
        std::ostringstream oss;
        oss << obj;
        return std::format_to(ctx.out(), "{}", oss.str());
    }
};
#pragma endregion

namespace Engine
{
    /// @defgroup Debug Debugging
    /// @brief Memory tracking, visual logging, and in-game overlays.

#pragma region ANSI Colors
    constexpr const char* COLOR_RESET = "\033[0m";
    constexpr const char* COLOR_RED = "\033[31m";
    constexpr const char* COLOR_GREEN = "\033[32m";
    constexpr const char* COLOR_YELLOW = "\033[33m";   // Orange/Yellow
    constexpr const char* COLOR_MAGENTA = "\033[35m";  // Purple
    constexpr const char* COLOR_CYAN = "\033[36m";     // Turquoise
    constexpr const char* COLOR_GRAY = "\033[90m";
#pragma endregion

    /// Defines the severity and filtering category of a log message.
    /// @ingroup Debug
    enum class LogLevel : uint16_t
    {
        None = 0,
        Error = 1 << 0,
        Warn = 1 << 1,
        Info = 1 << 2,
        Log = 1 << 3,
        Input = 1 << 4,
        InputMouse = 1 << 5,

        // Game Developer specific logs
        GameError = 1 << 6,
        GameWarn = 1 << 7,
        GameInfo = 1 << 8,

        All = 65535,

        // All, but turns off bit 5 (Mouse)
        NoMouseInput = 65535 & ~(1 << 5),

        // All, but turns off bits 4 and 5 (All Input)
        NoInput = 65535 & ~((1 << 4) | (1 << 5))
    };

    // Bitwise operators for LogLevel enum
    inline LogLevel operator|(LogLevel a, LogLevel b) { return static_cast<LogLevel>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b)); }
    inline LogLevel operator&(LogLevel a, LogLevel b) { return static_cast<LogLevel>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b)); }
    inline LogLevel operator~(LogLevel a) { return static_cast<LogLevel>(~static_cast<uint16_t>(a)); }

    /// Static logging system that handles console output, formatting, and writing to file.
    /// @ingroup Debug
    class Logger
    {
    public:
        Logger() = delete;

        /// Must be called at engine startup to enable ANSI colors in Windows and create the log file.
        static void Init();

        static void SetLevel(LogLevel level) { currentLevel = level; }
        static LogLevel GetLevel() { return currentLevel; }
        static void Enable(LogLevel level) { currentLevel = currentLevel | level; }
        static void Disable(LogLevel level) { currentLevel = currentLevel & ~level; }
        static bool Has(LogLevel level) { return static_cast<uint8_t>(currentLevel & level) != 0; }

        /// The master print function using C++20 std::format formatting.
        template <typename... Args>
        static void Print(LogLevel level, const char* colorCode, const char* prefix, std::format_string<Args...> fmt, Args&&... args)
        {
            if (!Has(level)) return;

            // Resolve the format string at the call site
            std::string formattedMsg = std::format(fmt, std::forward<Args>(args)...);

            // Delegate the actual I/O operations to the .cpp file
            PrintInternal(colorCode, prefix, formattedMsg);
        }

    private:
        /// Handles the final output to console, VS Output Window, and the log file.
        static void PrintInternal(const char* colorCode, const char* prefix, const std::string& message);

        static LogLevel currentLevel;
    };
}

#pragma region Debug Macros
// =========================================================================
// DEBUG MACROS (Using __VA_ARGS__)
// =========================================================================
#ifdef _DEBUG

// Engine Logs
#define ENGINE_ERROR(...)       Engine::Logger::Print(Engine::LogLevel::Error,      Engine::COLOR_RED,     "[ENGINE_ERROR]", __VA_ARGS__)
#define ENGINE_WARN(...)        Engine::Logger::Print(Engine::LogLevel::Warn,       Engine::COLOR_YELLOW,  "[ENGINE_WARN]",  __VA_ARGS__)
#define ENGINE_INFO(...)        Engine::Logger::Print(Engine::LogLevel::Info,       Engine::COLOR_GREEN,   "[ENGINE_INFO]",  __VA_ARGS__)
#define ENGINE_LOG(...)         Engine::Logger::Print(Engine::LogLevel::Log,        Engine::COLOR_GRAY,    "[ENGINE_LOG]",   __VA_ARGS__)
#define ENGINE_INPUT(...)       Engine::Logger::Print(Engine::LogLevel::Input,      Engine::COLOR_MAGENTA, "[INPUT]",        __VA_ARGS__)
#define ENGINE_INPUT_MOUSE(...) Engine::Logger::Print(Engine::LogLevel::InputMouse, Engine::COLOR_MAGENTA, "[IN_MOUSE]",     __VA_ARGS__)

// Game Logs (For the developer)
#define GAME_ERROR(...)         Engine::Logger::Print(Engine::LogLevel::GameError,  Engine::COLOR_RED,     "[GAME_ERROR]",   __VA_ARGS__)
#define GAME_WARN(...)          Engine::Logger::Print(Engine::LogLevel::GameWarn,   Engine::COLOR_YELLOW,  "[GAME_WARN]",    __VA_ARGS__)
#define GAME_INFO(...)          Engine::Logger::Print(Engine::LogLevel::GameInfo,   Engine::COLOR_CYAN,    "[GAME_INFO]",    __VA_ARGS__)

#else

#define ENGINE_ERROR(...)
#define ENGINE_WARN(...)
#define ENGINE_INFO(...)
#define ENGINE_LOG(...)
#define ENGINE_INPUT(...)
#define ENGINE_INPUT_MOUSE(...)

#define GAME_ERROR(...)
#define GAME_WARN(...)
#define GAME_INFO(...)

#endif
#pragma endregion