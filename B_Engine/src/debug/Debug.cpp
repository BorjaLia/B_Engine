#include "Debug.h"

#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Engine
{
    LogLevel Logger::currentLevel = LogLevel::All;

    // Declared internally to keep it isolated from other systems
    static std::ofstream logFile;

    void Logger::Init()
    {
        // 1. Open (or create) the engine.log file
        // std::ios::trunc ensures we clear the log from the previous session
        logFile.open("engine.log", std::ios::out | std::ios::trunc);
        if (logFile.is_open())
        {
            logFile << "========================================\n";
            logFile << "          ENGINE START                  \n";
            logFile << "========================================\n";
        }

        // 2. Windows requires manual configuration to understand ANSI colors
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE)
        {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode))
            {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
        }
#endif
    }

    void Logger::PrintOut(const std::string& message)
    {
#ifdef _WIN32
        OutputDebugStringA(message.c_str());
#endif

        if (logFile.is_open())
        {
            logFile << message;
            logFile.flush();
        }
    }
}