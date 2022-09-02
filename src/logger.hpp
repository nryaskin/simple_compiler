#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <chrono>
#include <sstream>
#include <source_location>

namespace logger {

enum log_level_t : char {
    Info = 'I',
    Warning = 'W',
    Error = 'E'
};

template<typename T>
void format_helper(std::ostringstream& oss, std::string_view& str, const T& value) {
    std::size_t open_bracket = str.find('{');
    if (open_bracket == std::string::npos) return;
    std::size_t close_bracket = str.find('}', open_bracket + 1);
    if (close_bracket == std::string::npos) return;
    oss << str.substr(0, open_bracket) << value;
    str = str.substr(close_bracket + 1);
}

template<typename... Targs>
std::string format(std::string_view str, Targs...args) {
    std::ostringstream oss;
    (format_helper(oss, str, args), ...);
    oss << str;
    return oss.str();
}

class SimpleLogger {
public:
    void log(log_level_t const level,
             std::string_view const message) {
        std::cout << format("[{}] {}",
                                 static_cast<char>(level),
                                 message) << '\n';

    }
};
}

#endif /* _LOGGER_H_ */
