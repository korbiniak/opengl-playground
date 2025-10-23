#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>

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

 public:
  Logger(std::ostream& stream = std::cout) : out(stream) {}

  void setLevel(LogLevel level) { minLevel = level; }

  template <typename... Args>
  void log(LogLevel level, Args&&... args) {
    if (level < minLevel) {
      return;
    }

    out << "[" << levelToString(level) << "] ";
    (out << ... << args);
    out << std::endl;
  }

  template <typename... Args>
  void info(Args&&... args) {
    log(LogLevel::INFO, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void error(Args&&... args) {
    log(LogLevel::ERROR, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void warning(Args&&... args) {
    log(LogLevel::WARNING, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void debug(Args&&... args) {
    log(LogLevel::DEBUG, std::forward<Args>(args)...);
  }
};

extern Logger Log;

#define LOG_DEBUG(...) \
  Log.log(LogLevel::DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define LOG_INFO(...) \
  Log.log(LogLevel::INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define LOG_WARNING(...) \
  Log.log(LogLevel::WARNING, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define LOG_ERROR(...) \
  Log.log(LogLevel::ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif /* LOGGER_H */
