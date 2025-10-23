#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

class ResourceException : public std::runtime_error {
 public:
  explicit ResourceException(const std::string& msg)
      : std::runtime_error(msg) {}
};

class ShaderCompilationException : public ResourceException {
 public:
  explicit ShaderCompilationException(const std::string& msg)
      : ResourceException(msg) {}
};

class FileNotFoundException : public ResourceException {
 public:
  explicit FileNotFoundException(const std::string& msg)
      : ResourceException(msg) {}
};

#endif /* EXCEPTIONS_H */
