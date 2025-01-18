# prettylog-cpp

A C++ logging library focused on readability in the console.
prettylog-cpp takes advantage of ANSI color codes to make your logs look ✨ ***pretty*** ✨.

## Installation

To integrate this single-header C++ library into your project, include the prettylog.hpp file:
```cpp
#include "prettylog.hpp"
```
Additionally, ensure that you link against `DbgHelp.lib` for stack tracing functionality.

## Logging

Logging is very easy. Just call the `log(message, type)` function. The type parameter is optional and defaults to `LogType::Runtime`.
```cpp
#include "prettylog.hpp"

int main() {
    PrettyLog::Logger::log("Hello there!", PrettyLog::CustomLogType{"Information", {PrettyLog::AnsiColor::BLUE, PrettyLog::AnsiColor::WHITE}});
    PrettyLog::Logger::log("general kenobi", PrettyLog::CustomLogType{"Network", {PrettyLog::AnsiColor::CYAN, PrettyLog::AnsiColor::WHITE}});
}
```
You can also log exceptions!
```cpp
#include <stdexcept>

int main() {
    try {
        // Some code that might throw an error
        throw std::runtime_error("An error occurred");
    } catch (const std::exception& e) {
        PrettyLog::Logger::logException(e);
    }
}
```

## Logger Settings
You can change settings by modifying the `LoggerSettings`:
```cpp
#include "prettylog.hpp"

int main() {
    PrettyLog::LoggerSettings::saveToFile = true;
    PrettyLog::LoggerSettings::saveDirectoryPath = "./logs/";

    PrettyLog::Logger::log("This message is saved to a file", PrettyLog::CustomLogType{"Info", {PrettyLog::AnsiColor::GREEN, PrettyLog::AnsiColor::WHITE}});
}
```

## Log Types
There are various default log types: **Debug, Information, Runtime, Network, Success, Warning, Error, Exception, Critical, Audit, Trace, Security, User Action, Performance, Config,** and **Fatal.**

## Custom Log Types
You can create custom log types by implementing your own enum and associated functions.
```cpp
#include "prettylog.hpp"

enum class MyCustomLogType {
    Cute,
    Git,
    FireWarning
};

namespace PrettyLog {
    template <>
    CustomLogType getLogType<MyCustomLogType>(MyCustomLogType type) {
        switch (type) {
            case MyCustomLogType::Cute:
                return CustomLogType{"≽^•⩊•^≼", {AnsiColor::MAGENTA, AnsiColor::WHITE}};
            case MyCustomLogType::Git:
                return CustomLogType{"🤖 Git", {AnsiColor::YELLOW, AnsiColor::WHITE}};
            case MyCustomLogType::FireWarning:
                return CustomLogType{"🔥 Fire Warning", {AnsiColor::RED, AnsiColor::WHITE}};
            default:
                return CustomLogType{"Unknown", {AnsiColor::RESET, AnsiColor::RESET}};
        }
    }
}

int main() {
    PrettyLog::Logger::log("T-This is a cute message", PrettyLog::getLogType(MyCustomLogType::Cute));
    PrettyLog::Logger::log("Refusing to merge unrelated histories", PrettyLog::getLogType(MyCustomLogType::Git));
    PrettyLog::Logger::log("SERVER ROOM ON FIRE", PrettyLog::getLogType(MyCustomLogType::FireWarning));
}
```

## License
This project is licensed under the MIT License - see the [LICENSE](https://github.com/selenagomez25/prettylog-cpp/blob/main/LICENSE) file for details.

## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.

## Credits
https://github.com/LukynkaCZE/PrettyLog
