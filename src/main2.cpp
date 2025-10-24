#include <array>
#include <iostream>

#include <cxxopts.hpp>
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_iostream.hpp>

#include "src/application.h"
#include "src/logger.h"

template <class T>
std::string join(const T& strs) {
  std::string result;
  for (size_t i = 0; i < strs.size(); i++) {
    if (i)
      result += "|";
    result += strs[i];
  }
  return result;
}

using magic_enum::iostream_operators::operator>>;

int main(int argc, char* argv[]) {
  cxxopts::Options options("Engine", "Toy OpenGL engine");

  constexpr auto logLevels = magic_enum::enum_names<LogLevel>();

  options.add_options()("l,log-level",
                        "Log level, possible values: " + join(logLevels),
                        cxxopts::value<LogLevel>()->default_value("INFO"))(
      "h,help", "Print usage");

  cxxopts::ParseResult result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    return 0;
  }

  Application app;
  Log.setLevel(LogLevel::DEBUG);

  app.init();
  app.run();
  return 0;
}
