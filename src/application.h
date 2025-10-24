#ifndef APPLICATION_H
#define APPLICATION_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "src/resource_manager.h"
#include "src/scene.h"

class Application {
 private:
  GLFWwindow* window;
  int width, height;

  float deltaTime;
  float lastFrame;
  bool firstMouse;
  float lastX, lastY;
  float mouseSensitivity;

  bool keys[1024] = {false};

  Scene scene;
  ResourceManager resourceManager;

  static void keyCallbackStatic(GLFWwindow* window, int key, int scancode,
                                int action, int mods);

  static void mouseCallbackStatic(GLFWwindow* window, double xpos, double ypos);

  static void framebufferSizeCallbackStatic(GLFWwindow* window, int xpos,
                                            int ypos);

  static void windowPosCallbackStatic(GLFWwindow* window, int xpos, int ypos);

  void keyCallback(int key, int scancode, int action, int mods);

  void mouseCallback(double xpos, double ypos);

  void framebufferSizeCallback(int xpos, int ypos);

  void windowPosCallback(int xpos, int ypos);

  void processInput();

  float getAspectRatio();

  void initGLFW();

  void initGL();

  void loadResources();

  void setupScene();

  void update();

  void render();

 public:
  Application(int width = 800, int height = 600);

  ~Application();

  void init();

  void run();
};

#endif /* APPLICATION_H */
