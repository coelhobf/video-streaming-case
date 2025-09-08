#pragma once

#include <string>
#include <format>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace paladium {

class Logger {
public:
    enum class Level { DEBUG, INFO, WARN, ERROR };

    template<typename... Args>
    static void debug(std::format_string<Args...> format, Args&&... args) {
        log(Level::DEBUG, std::format(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    static void info(std::format_string<Args...> format, Args&&... args) {
        log(Level::INFO, std::format(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    static void warn(std::format_string<Args...> format, Args&&... args) {
        log(Level::WARN, std::format(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    static void error(std::format_string<Args...> format, Args&&... args) {
        log(Level::ERROR, std::format(format, std::forward<Args>(args)...));
    }

private:
    static void log(Level level, const std::string& message) {
        std::cout << std::format("[{}] {}: {}\n", 
                                get_timestamp(), 
                                level_to_string(level), 
                                message);
        std::cout.flush();
    }

    static std::string get_timestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    static std::string level_to_string(Level level) {
        switch (level) {
            case Level::DEBUG: return "DEBUG";
            case Level::INFO:  return "INFO ";
            case Level::WARN:  return "WARN ";
            case Level::ERROR: return "ERROR";
            default:           return "UNKNOWN";
        }
    }
};

} // namespace paladium
