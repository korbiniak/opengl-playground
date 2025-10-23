#include "src/application.h"
#include "src/logger.h"

int main() {
  Application app;
  Log.setLevel(LogLevel::DEBUG);

  app.init();
  app.run();
  return 0;
}
