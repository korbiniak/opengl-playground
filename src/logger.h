#ifndef LOGGER_H
#define LOGGER_H

#include <experimental/source_location>
#include <iostream>
#include <string_view>

#include <glm/glm.hpp>

inline std::ostream& operator<<(std::ostream& os, const glm::vec2& v) {
  return os << "[" << v.x << ", " << v.y << "]";
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
  return os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec4& v) {
  return os << "[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]";
}

enum class LogLevel {
  DEBUG,
  INFO,
  WARNING,
  ERROR,
};

class Logger {
 private:
  LogLevel minLevel = LogLevel::INFO;
  std::ostream& out;

  static const char* levelToString(LogLevel level) {
    switch (level) {
      case LogLevel::DEBUG:
        return "DEBUG";
      case LogLevel::INFO:
        return "INFO";
      case LogLevel::WARNING:
        return "WARNING";
      case LogLevel::ERROR:
        return "ERROR";
      default:
        return "UNKNOWN";
    }
  }

  // Extract just the filename from full path
  static std::string_view getFilename(std::string_view path) {
    auto pos = path.find_last_of("/\\");
    return pos == std::string_view::npos ? path : path.substr(pos + 1);
  }

 public:
  Logger(std::ostream& stream = std::cout) : out(stream) {}

  void setLevel(LogLevel level) { minLevel = level; }

  template <typename... Args>
  void log(LogLevel level, const std::experimental::source_location& location,
           Args&&... args) {
    if (level < minLevel) {
      return;
    }

    out << "[" << levelToString(level) << "] "
        << getFilename(location.file_name()) << ":" << location.line() << " "
        << location.function_name() << "() - ";

    (out << ... << args);
    out << std::endl;
  }

  template <typename... Args>
  void info(const std::experimental::source_location& location =
                std::experimental::source_location::current(),
            Args&&... args) {
    log(LogLevel::INFO, location, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void error(Args&&... args,
             const std::experimental::source_location& location =
                 std::experimental::source_location::current()) {
    log(LogLevel::ERROR, location, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void warning(Args&&... args,
               const std::experimental::source_location& location =
                   std::experimental::source_location::current()) {
    log(LogLevel::WARNING, location, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void debug(Args&&... args,
             const std::experimental::source_location& location =
                 std::experimental::source_location::current()) {
    log(LogLevel::DEBUG, location, std::forward<Args>(args)...);
  }
};

extern Logger Log;

// Macros that capture source location
#define LOG_DEBUG(...)                                                    \
  Log.log(LogLevel::DEBUG, std::experimental::source_location::current(), \
          __VA_ARGS__)

#define LOG_INFO(...)                                                    \
  Log.log(LogLevel::INFO, std::experimental::source_location::current(), \
          __VA_ARGS__)

#define LOG_WARNING(...)                                                    \
  Log.log(LogLevel::WARNING, std::experimental::source_location::current(), \
          __VA_ARGS__)

#define LOG_ERROR(...)                                                    \
  Log.log(LogLevel::ERROR, std::experimental::source_location::current(), \
          __VA_ARGS__)

#endif /* LOGGER_H */
