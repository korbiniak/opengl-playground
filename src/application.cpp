#include "src/application.h"

#include <memory>
#include <numeric>
#include <stdexcept>

#include <GLFW/glfw3.h>

#include "src/camera.h"
#include "src/circular_motion_component.h"
#include "src/game_object.h"
#include "src/light_source.h"
#include "src/logger.h"
#include "src/utils.h"
#include "src/vertex.h"

void Application::keyCallbackStatic(GLFWwindow* window, int key, int scancode,
                                    int action, int mods) {
  Application* app =
      static_cast<Application*>(glfwGetWindowUserPointer(window));
  app->keyCallback(key, scancode, action, mods);
}

void Application::mouseCallbackStatic(GLFWwindow* window, double xpos,
                                      double ypos) {
  Application* app =
      static_cast<Application*>(glfwGetWindowUserPointer(window));
  app->mouseCallback(xpos, ypos);
}

void Application::framebufferSizeCallbackStatic(GLFWwindow* window, int xpos,
                                                int ypos) {
  Application* app =
      static_cast<Application*>(glfwGetWindowUserPointer(window));
  app->framebufferSizeCallback(xpos, ypos);
}

void Application::windowPosCallbackStatic(GLFWwindow* window, int xpos,
                                          int ypos) {
  Application* app =
      static_cast<Application*>(glfwGetWindowUserPointer(window));
  app->windowPosCallback(xpos, ypos);
}

void Application::keyCallback(int key, int scancode, int action, int mods) {
  (void)scancode;
  (void)mods;
  if (key >= 0 && key < 1024) {
    LOG_DEBUG("Key pressed: ", (char)key);
    keys[key] = action;
  }
}

void Application::mouseCallback(double xpos, double ypos) {
  LOG_DEBUG("Mouse changed position: ", xpos, ", ", ypos);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = (xpos - lastX) * mouseSensitivity;
  float yoffset = (ypos - lastY) * mouseSensitivity;

  lastX = xpos;
  lastY = ypos;

  scene.getActiveCamera().rotate(xoffset, -yoffset);
}

void Application::framebufferSizeCallback(int xpos, int ypos) {
  LOG_DEBUG("Changing window size ", xpos, ", ", ypos);
  glViewport(0, 0, xpos, ypos);
  width = xpos;
  height = ypos;
  firstMouse = true;
  for (Camera& camera : scene.getAllCameras()) {
    camera.setAspectRatio(getAspectRatio());
  }
}

void Application::windowPosCallback(int xpos, int ypos) {
  LOG_DEBUG("Window position changed: ", xpos, ", ", ypos);
  firstMouse = true;
}

void Application::processInput() {
  LOG_DEBUG("Processing input");

  if (keys[GLFW_KEY_ESCAPE]) {
    glfwSetWindowShouldClose(window, true);
  }

  Camera& camera = scene.getActiveCamera();
  if (keys[GLFW_KEY_W]) {
    camera.moveForward(deltaTime);
  }
  if (keys[GLFW_KEY_S]) {
    camera.moveBackward(deltaTime);
  }
  if (keys[GLFW_KEY_A]) {
    camera.moveLeft(deltaTime);
  }
  if (keys[GLFW_KEY_D]) {
    camera.moveRight(deltaTime);
  }
  if (keys[GLFW_KEY_SPACE]) {
    camera.moveUp(deltaTime);
  }
  if (keys[GLFW_KEY_LEFT_SHIFT]) {
    camera.moveDown(deltaTime);
  }
  if (keys[GLFW_KEY_TAB] && !wireframe) {
    wireframe = true;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    wireframe = false;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void Application::initGLFW() {
  if (!glfwInit()) {
    LOG_ERROR("glfwInit failed");
    throw std::runtime_error("glfwInit failed");
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(width, height, "Main", NULL, NULL);

  if (window == nullptr) {
    LOG_ERROR("Window creation failed");
    glfwTerminate();
    throw std::runtime_error("Window creation failed");
  }

  glfwMakeContextCurrent(window);
  glfwSetWindowUserPointer(window, this);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwSetCursorPosCallback(window, mouseCallbackStatic);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallbackStatic);
  glfwSetKeyCallback(window, keyCallbackStatic);
  glfwSetWindowPosCallback(window, windowPosCallbackStatic);
}

void Application::initGL() {
  if (glewInit() != GLEW_OK) {
    LOG_ERROR("glewInit failed");
    throw std::runtime_error("glewInit failed");
  }

  GLint nrAttributes;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
  LOG_INFO("Maximum nr of vertex attributes supported: ", nrAttributes);

  glViewport(0, 0, width, height);
  glEnable(GL_DEPTH_TEST);
}

void Application::loadResources() {
  resourceManager.loadShader("shader", "shaders/light.vert",
                             "shaders/light.frag");

  resourceManager.loadShader("lightSourceShader", "shaders/light.vert",
                             "shaders/light_src.frag");

  resourceManager.loadShader("fontShader", "shaders/font.vert",
                             "shaders/font.frag");

  resourceManager.loadTexture("texture", "assets/container.jpg");
  resourceManager.loadFont("arial", "fonts/arial.ttf", 64);
  resourceManager.loadFont("shiny", "fonts/shiny.ttf", 64);

  std::vector<Vertex> vertices = {
      {{-0.5F, -0.5F, -0.5F}, {0.0F, 0.0F, -1.0F}, {0.0F, 0.0F}},
      {{0.5F, -0.5F, -0.5F}, {0.0F, 0.0F, -1.0F}, {1.0F, 0.0F}},
      {{0.5F, 0.5F, -0.5F}, {0.0F, 0.0F, -1.0F}, {1.0F, 1.0F}},
      {{0.5F, 0.5F, -0.5F}, {0.0F, 0.0F, -1.0F}, {1.0F, 1.0F}},
      {{-0.5F, 0.5F, -0.5F}, {0.0F, 0.0F, -1.0F}, {0.0F, 1.0F}},
      {{-0.5F, -0.5F, -0.5F}, {0.0F, 0.0F, -1.0F}, {0.0F, 0.0F}},

      {{-0.5F, -0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}, {0.0F, 0.0F}},
      {{0.5F, -0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}, {1.0F, 0.0F}},
      {{0.5F, 0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}, {1.0F, 1.0F}},
      {{0.5F, 0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}, {1.0F, 1.0F}},
      {{-0.5F, 0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}, {0.0F, 1.0F}},
      {{-0.5F, -0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}, {0.0F, 0.0F}},

      {{-0.5F, 0.5F, 0.5F}, {-1.0F, 0.0F, 0.0F}, {1.0F, 0.0F}},
      {{-0.5F, 0.5F, -0.5F}, {-1.0F, 0.0F, 0.0F}, {1.0F, 1.0F}},
      {{-0.5F, -0.5F, -0.5F}, {-1.0F, 0.0F, 0.0F}, {0.0F, 1.0F}},
      {{-0.5F, -0.5F, -0.5F}, {-1.0F, 0.0F, 0.0F}, {0.0F, 1.0F}},
      {{-0.5F, -0.5F, 0.5F}, {-1.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
      {{-0.5F, 0.5F, 0.5F}, {-1.0F, 0.0F, 0.0F}, {1.0F, 0.0F}},

      {{0.5F, 0.5F, 0.5F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F}},
      {{0.5F, 0.5F, -0.5F}, {1.0F, 0.0F, 0.0F}, {1.0F, 1.0F}},
      {{0.5F, -0.5F, -0.5F}, {1.0F, 0.0F, 0.0F}, {0.0F, 1.0F}},
      {{0.5F, -0.5F, -0.5F}, {1.0F, 0.0F, 0.0F}, {0.0F, 1.0F}},
      {{0.5F, -0.5F, 0.5F}, {1.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
      {{0.5F, 0.5F, 0.5F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F}},

      {{-0.5F, -0.5F, -0.5F}, {0.0F, -1.0F, 0.0F}, {0.0F, 1.0F}},
      {{0.5F, -0.5F, -0.5F}, {0.0F, -1.0F, 0.0F}, {1.0F, 1.0F}},
      {{0.5F, -0.5F, 0.5F}, {0.0F, -1.0F, 0.0F}, {1.0F, 0.0F}},
      {{0.5F, -0.5F, 0.5F}, {0.0F, -1.0F, 0.0F}, {1.0F, 0.0F}},
      {{-0.5F, -0.5F, 0.5F}, {0.0F, -1.0F, 0.0F}, {0.0F, 0.0F}},
      {{-0.5F, -0.5F, -0.5F}, {0.0F, -1.0F, 0.0F}, {0.0F, 1.0F}},

      {{-0.5F, 0.5F, -0.5F}, {0.0F, 1.0F, 0.0F}, {0.0F, 1.0F}},
      {{0.5F, 0.5F, -0.5F}, {0.0F, 1.0F, 0.0F}, {1.0F, 1.0F}},
      {{0.5F, 0.5F, 0.5F}, {0.0F, 1.0F, 0.0F}, {1.0F, 0.0F}},
      {{0.5F, 0.5F, 0.5F}, {0.0F, 1.0F, 0.0F}, {1.0F, 0.0F}},
      {{-0.5F, 0.5F, 0.5F}, {0.0F, 1.0F, 0.0F}, {0.0F, 0.0F}},
      {{-0.5F, 0.5F, -0.5F}, {0.0F, 1.0F, 0.0F}, {0.0F, 1.0F}},
  };

  std::vector<unsigned int> indices(vertices.size());
  iota(indices.begin(), indices.end(), 0);
  LOG_INFO(indices[0], indices[1]);

  resourceManager.loadMesh("cube", vertices, indices);
}

float Application::getAspectRatio() {
  return (float)width / height;
}

void Application::setupScene() {
  std::shared_ptr<Material> cubeMaterial = std::make_unique<Material>(
      resourceManager.getShader("shader"), nullptr, glm::vec3(1.0F));

  std::shared_ptr<Material> lightSourceMaterial = std::make_unique<Material>(
      resourceManager.getShader("lightSourceShader"), nullptr, glm::vec3(1.0F));

  scene.addCamera(Camera(glm::vec3(0.0f, 0.0f, 3.0f), getAspectRatio()));

  std::unique_ptr<GameObject> cube = std::make_unique<GameObject>(
      resourceManager.getMesh("cube"), cubeMaterial);
  cube->setPosition(glm::vec3(0.0F, 0.0F, 0.0F));

  std::unique_ptr<LightSource> light = std::make_unique<LightSource>(
      resourceManager.getMesh("cube"), lightSourceMaterial,
      glm::vec3(2.0, 1.0, 1.0), 0.5F);
  light->setPosition(glm::vec3(1.0F, 1.4F, 1.3F));
  light->setScale(glm::vec3(0.1));
  std::unique_ptr<CircularMotionComponent> circularMotion =
      std::make_unique<CircularMotionComponent>(glm::vec3(1.0F), 2.0F, 1.0F,
                                                glm::vec3(1.0F, 0.0F, 0.0F));
  light->addComponent(std::move(circularMotion));

  std::unique_ptr<LightSource> light2 = std::make_unique<LightSource>(
      resourceManager.getMesh("cube"), lightSourceMaterial,
      glm::vec3(0.3, 0.8, 0.1), 0.5F);
  light2->setPosition(glm::vec3(1.0F, 1.4F, 1.3F));
  light2->setScale(glm::vec3(0.1));
  std::unique_ptr<CircularMotionComponent> circularMotion2 =
      std::make_unique<CircularMotionComponent>(
          glm::vec3(1.0, 1.0, 1.0), 2.0F, 0.5F, glm::vec3(0.0F, 1.0F, 0.0F));
  light2->addComponent(std::move(circularMotion2));

  scene.addObject(std::move(cube));
  scene.addLightSource(std::move(light));
  scene.addLightSource(std::move(light2));
}

void Application::setupUI() {}

void Application::update() {
  LOG_DEBUG("Update");
  float currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  processInput();
  scene.update(deltaTime);
}

void Application::render() {
  LOG_DEBUG("Render");
  glClearColor(0.1F, 0.1F, 0.1F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  std::shared_ptr<Shader> fontShader = resourceManager.getShader("fontShader");
  scene.render();
  ui.render(*fontShader, width, height);
}

Application::Application(int width, int height)
    : width(width),
      height(height),
      deltaTime(0.0F),
      lastFrame(0.0F),
      firstMouse(true),
      lastX(width / 2.0),
      lastY(height / 2.0),
      mouseSensitivity(0.1F),
      wireframe(false) {}

void Application::init() {
  initGLFW();
  initGL();

  loadResources();
  checkGLError("After loading resources");
  setupScene();
  checkGLError("After setting up scene");
  setupUI();
  checkGLError("After setting up UI");

  LOG_INFO("Initialization complete.");
}

void Application::run() {
  while (!glfwWindowShouldClose(window)) {
    update();
    checkGLError("After update");
    render();
    checkGLError("After render");

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

Application::~Application() {
  glfwTerminate();
}
