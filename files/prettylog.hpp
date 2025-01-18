#ifndef PRETTYLOG_HPP
#define PRETTYLOG_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <mutex>
#include <map>
#include <fstream>
#include <filesystem>
#include <vector>
#include <exception>
#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#else
#include <execinfo.h>
#endif

namespace PrettyLog {

    struct AnsiColor {
        static constexpr const char* RESET = "\033[0m";
        static constexpr const char* BLACK = "\033[30m";
        static constexpr const char* RED = "\033[31m";
        static constexpr const char* GREEN = "\033[32m";
        static constexpr const char* YELLOW = "\033[33m";
        static constexpr const char* BLUE = "\033[34m";
        static constexpr const char* MAGENTA = "\033[35m";
        static constexpr const char* CYAN = "\033[36m";
        static constexpr const char* WHITE = "\033[37m";
        static constexpr const char* BOLD = "\033[1m";
    };

    struct AnsiPair {
        std::string background;
        std::string foreground;
    };

    struct CustomLogType {
        std::string name;
        AnsiPair colorPair;
    };

    class LoggerSettings {
    public:
        static inline bool saveToFile = true;
        static inline std::string saveDirectoryPath = "./logs/";

        static void load() {
            if (!std::filesystem::exists(saveDirectoryPath)) {
                std::filesystem::create_directories(saveDirectoryPath);
            }
            FILE* file;
            errno_t err = fopen_s(&file, getLogFilePath().c_str(), "a");
            if (err != 0 || !file) {
                printf("Failed to create log file.\n");
            }
            else {
                fclose(file);
            }
        }

    private:
        static std::string getLogFilePath() {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::tm localTime;
            localtime_s(&localTime, &time);
            std::ostringstream oss;
            oss << std::put_time(&localTime, "%Y-%m-%d-%H%M%S") << ".log";
            return saveDirectoryPath + "/" + oss.str();
        }
    };

    class Logger {
    public:
        static void log(const std::string& message, const CustomLogType& type) {
            std::lock_guard<std::mutex> lock(logMutex);
            auto formattedMessage = formatMessage(message, type);
            printf("%s\n", formattedMessage.c_str());

            if (LoggerSettings::saveToFile) {
                writeToFile(message, type);
            }
        }

        static void logException(const std::exception& e) {
            log(e.what(), { "Exception", {AnsiColor::BLACK, AnsiColor::RED} });
            for (const auto& trace : getStackTrace()) {
                log(trace, { "Stack Trace", {AnsiColor::BLACK, AnsiColor::YELLOW} });
            }
        }

    private:
        static std::mutex logMutex;

        static void writeToFile(const std::string& message, const CustomLogType& type) {
            std::string logFilePath = LoggerSettings::saveDirectoryPath + "/log.txt";
            FILE* file;
            errno_t err = fopen_s(&file, logFilePath.c_str(), "a");

            if (err != 0 || !file) {
                printf("Failed to open log file: %s\n", logFilePath.c_str());
                return;
            }

            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::tm localTime;
            localtime_s(&localTime, &time);

            fprintf(file, "[%s] [%s] %s\n",
                std::put_time(&localTime, "%Y-%m-%d %H:%M:%S"),
                type.name.c_str(),
                message.c_str());

            fclose(file);
        }

        static std::string formatMessage(const std::string& message, const CustomLogType& type) {
            return type.colorPair.foreground + type.name + ": " + message + AnsiColor::RESET;
        }

        static std::vector<std::string> getStackTrace() {
            constexpr size_t maxFrames = 64;
            std::vector<std::string> stackTrace;

#ifdef _WIN32
            void* stack[maxFrames];
            USHORT frames = CaptureStackBackTrace(0, maxFrames, stack, nullptr);

            HANDLE process = GetCurrentProcess();
            SymInitialize(process, nullptr, TRUE);

            char symbolBuffer[sizeof(SYMBOL_INFO) + 256 * sizeof(char)] = {};
            SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
            symbol->MaxNameLen = 255;
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

            for (USHORT i = 0; i < frames; ++i) {
                SymFromAddr(process, (DWORD64)(stack[i]), nullptr, symbol);
                std::ostringstream oss;
                oss << symbol->Name << " [0x" << std::hex << symbol->Address << "]";
                stackTrace.emplace_back(oss.str());
            }

            SymCleanup(process);
#else
            void* buffer[maxFrames];
            int frames = backtrace(buffer, maxFrames);
            char** symbols = backtrace_symbols(buffer, frames);

            for (int i = 0; i < frames; ++i) {
                stackTrace.emplace_back(symbols[i]);
            }
            free(symbols);
#endif
            return stackTrace;
        }
    };

    std::mutex Logger::logMutex;

} // namespace PrettyLog

#endif // PRETTYLOG_HPP
