#include "src/application.h"

#include <memory>
#include <numeric>
#include <stdexcept>

#include <GLFW/glfw3.h>

#include "src/camera.h"
#include "src/circular_motion_component.h"
#include "src/directional_light_component.h"
#include "src/game_object.h"
#include "src/light_component.h"
#include "src/logger.h"
#include "src/point_light_component.h"
#include "src/rainbow_component.h"
#include "src/rotation_component.h"
#include "src/spotlight_component.h"
#include "src/utils.h"
#include "src/vertex.h"
#include "src/world_text.h"

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

  scene.getActiveCamera()->rotate(xoffset, -yoffset);
}

void Application::framebufferSizeCallback(int xpos, int ypos) {
  LOG_DEBUG("Changing window size ", xpos, ", ", ypos);
  glViewport(0, 0, xpos, ypos);
  width = xpos;
  height = ypos;
  firstMouse = true;
  for (Camera* camera : scene.getAllCameras()) {
    camera->setAspectRatio(getAspectRatio());
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

  Camera* camera = scene.getActiveCamera();
  if (keys[GLFW_KEY_W]) {
    camera->moveForward(deltaTime);
  }
  if (keys[GLFW_KEY_S]) {
    camera->moveBackward(deltaTime);
  }
  if (keys[GLFW_KEY_A]) {
    camera->moveLeft(deltaTime);
  }
  if (keys[GLFW_KEY_D]) {
    camera->moveRight(deltaTime);
  }
  if (keys[GLFW_KEY_SPACE]) {
    camera->moveUp(deltaTime);
  }
  if (keys[GLFW_KEY_LEFT_SHIFT]) {
    camera->moveDown(deltaTime);
  }
  if (keys[GLFW_KEY_TAB] && !wireframe) {
    wireframe = true;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  if (!keys[GLFW_KEY_TAB] && wireframe) {
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
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::loadResources() {
  resourceManager.loadShader("shader", "shaders/light.vert",
                             "shaders/light.frag");

  resourceManager.loadShader("lightSourceShader", "shaders/light.vert",
                             "shaders/light_src.frag");

  resourceManager.loadShader("fontShader", "shaders/font.vert",
                             "shaders/font.frag");

  resourceManager.loadShader("3dFontShader", "shaders/light.vert",
                             "shaders/light_font.frag");

  resourceManager.loadShader("3dBrightFontShader", "shaders/light.vert",
                             "shaders/bright_font.frag");

  resourceManager.loadTexture("texture", "assets/container.jpg");
  resourceManager.loadTexture("container2", "assets/container2.png");
  resourceManager.loadTexture("container2_specular",
                              "assets/container2_specular.png");

  resourceManager.loadFont("arial", "fonts/arial.ttf", 64);
  resourceManager.loadFont("shiny", "fonts/shiny.ttf", 64);
  resourceManager.loadFont("superpower", "fonts/superpower.ttf", 64);

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

  resourceManager.createMaterial("container2", "shader", "container2",
                                 "container2_specular");
}

float Application::getAspectRatio() {
  return (float)width / height;
}

void Application::setupScene() {

  std::unique_ptr<Camera> camera =
      std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f), getAspectRatio());

  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

  for (size_t i = 0; i < 10; i++) {
    auto textMaterial =
        std::make_shared<Material>(resourceManager.getShader("3dFontShader"),
                                   resourceManager.getFont("arial"), nullptr);
    textMaterial->setBaseColor(glm::vec3(1.0F));
    textMaterial->setOpaque(false);

    auto text = std::make_unique<WorldText>(resourceManager.getFont("arial"),
                                            std::move(textMaterial),
                                            "Michał jest super :)");
    text->setName("text" + std::to_string(i));
    text->setScale(glm::vec3(0.001F));
    text->setPosition(glm::vec3(-0.25F, 0.0F, 0.501F));

    std::unique_ptr<GameObject> cube =
        std::make_unique<GameObject>(resourceManager.getMesh("cube"),
                                     resourceManager.getMaterial("container2"));
    cube->setPosition(cubePositions[i]);
    cube->rotate(20.0F * i, glm::vec3(1.0F, 0.3F, 0.5F));
    cube->addChild(std::move(text));
    scene.addObject(std::move(cube));
  }

  std::shared_ptr<Material> dirLightMaterial = std::make_unique<Material>(
      resourceManager.getShader("lightSourceShader"), nullptr);
  std::unique_ptr<GameObject> dirLight = std::make_unique<GameObject>(
      resourceManager.getMesh("cube"), dirLightMaterial);
  dirLightMaterial->setBaseColor(glm::vec3(1.0F));
  dirLight->setPosition(glm::vec3(0.0F, 0.0F, -200.0F));
  dirLight->setScale(glm::vec3(10.0));
  dirLight->faceDirection(glm::vec3(1.0F));

  std::unique_ptr<DirectionalLightComponent> dirLightComponent =
      std::make_unique<DirectionalLightComponent>(
          glm::vec3(0.1F), glm::vec3(1.0F), glm::vec3(1.0F),
          glm::vec3(-1.0F, -1.0F, -1.0F));
  dirLight->addComponent(std::move(dirLightComponent));

  std::shared_ptr<Material> pointLightMaterial = std::make_unique<Material>(
      resourceManager.getShader("lightSourceShader"), nullptr);
  std::unique_ptr<GameObject> pointLight = std::make_unique<GameObject>(
      resourceManager.getMesh("cube"), pointLightMaterial);
  pointLight->setPosition(glm::vec3(1.0F, 1.4F, 1.3F));
  pointLight->setScale(glm::vec3(0.1));

  std::unique_ptr<PointLightComponent> pointLightComponent =
      std::make_unique<PointLightComponent>();
  pointLightComponent->setDiffuse(glm::vec3(1.0F, 0.0F, 0.0F));
  pointLightComponent->setAmbient(glm::vec3(0.0F));
  std::unique_ptr<CircularMotionComponent> circularMotion2 =
      std::make_unique<CircularMotionComponent>(
          glm::vec3(0.0, 1.0, 0.0), 2.0F, 1.0F, glm::vec3(0.0F, 1.0F, 0.0F));
  pointLight->addComponent(std::move(circularMotion2));
  pointLight->addComponent(std::move(pointLightComponent));

  std::unique_ptr<SpotlightComponent> spotLightComponent =
      std::make_unique<SpotlightComponent>(glm::vec3(0.0F, 0.0F, -1.0F));
  spotLightComponent->setDiffuse(glm::vec3(0.0F, 1.0F, 0.0F));
  spotLightComponent->setCutOff(20.0F, 25.0F);

  std::shared_ptr<Material> spotLightMaterial = std::make_unique<Material>(
      resourceManager.getShader("lightSourceShader"), nullptr);
  std::unique_ptr<GameObject> spotLight = std::make_unique<GameObject>(
      resourceManager.getMesh("cube"), spotLightMaterial);
  spotLight->addComponent(std::move(spotLightComponent));
  spotLight->faceDirection(glm::vec3(0.0F, 0.0F, -1.0F));
  spotLight->setScale(glm::vec3(0.1F, 0.1F, 0.3F));
  spotLight->setPosition(glm::vec3(0.2F, -0.2F, -1.0F));
  camera->addChild(std::move(spotLight));

  auto bannerMaterial = std::make_shared<Material>(
      resourceManager.getShader("3dBrightFontShader"),
      resourceManager.getFont("superpower"), nullptr);
  bannerMaterial->setBaseColor(glm::vec3(1.0F));
  bannerMaterial->setOpaque(false);

  auto bannerPivot = std::make_unique<GameObject>(nullptr, nullptr);
  bannerPivot->setPosition(glm::vec3(10.0F, 0.0F, 30.0F));
  bannerPivot->addComponent(
      std::make_unique<RotationComponent>(glm::vec3(0.0F, 1.0F, 0.0F), 60.F));

  auto bannerText = std::make_unique<WorldText>(
      resourceManager.getFont("superpower"), std::move(bannerMaterial),
      "Michał to pała", true);

  bannerText->setScale(glm::vec3(0.1F));
  bannerText->faceDirection(glm::vec3(0.0F, 0.0F, 1.0F));
  bannerText->setPosition(glm::vec3(20.0F, 0.0F, 0.0F));
  bannerText->addComponent(std::make_unique<RainbowComponent>(100.0F));

  bannerPivot->addChild(std::move(bannerText));

  scene.addObject(std::move(dirLight));
  scene.addObject(std::move(pointLight));
  scene.addObject(std::move(bannerPivot));
  scene.addCamera(std::move(camera));
}

void Application::setupUI() {
  // ui.addText("title", resourceManager.getFont("arial"), glm::vec2(25.0F, 25.0F),
  //            "Fajna giera", 1.0F);
}

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
  ui.render(fontShader.get(), width, height);
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
