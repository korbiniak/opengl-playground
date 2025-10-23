#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glm/ext/matrix_transform.hpp"
#include "src/shader.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
  (void)window;
  glViewport(0, 0, width, height);
}

glm::vec3 cameraPos = glm::vec3(0.0F, 0.0F, 3.0F);
glm::vec3 cameraFront = glm::vec3(0.0F, 0.0F, -1.0F);
glm::vec3 cameraUp = glm::vec3(0.0F, 1.0F, 0.0F);

float deltaTime = 0.0F;
float yaw = 0.0F, pitch = 0.0F;
float lastX = 400, lastY = 300;

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  const float cameraSpeed = 2.5F * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos +=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    cameraPos += cameraSpeed * glm::vec3(0.0F, 1.0F, 0.0F);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    cameraPos += cameraSpeed * glm::vec3(0.0F, -1.0F, 0.0F);
  }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
  (void)window;
  float xoffset = xpos - lastX;
  float yoffset = ypos - lastY;

  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.1F;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch -= yoffset;

  if (pitch > 89.0F)
    pitch = 89.0F;
  if (pitch < -89.0F)
    pitch = -89.0F;

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(direction);
}

struct Character {
  unsigned int textureId;
  glm::ivec2 size;
  glm::ivec2 bearing;
  FT_Pos advance;
};

std::map<char, Character> characters;

void initFonts() {
  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  FT_Face face;
  if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) {
    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    exit(EXIT_FAILURE);
  }

  FT_Set_Pixel_Sizes(face, 0, 128);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  FT_GlyphSlot slot = face->glyph;

  for (unsigned char c = 31; c < 128; c++) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cout << "ERROR::FREETYPE: Failed to load Glyph for character " << c
                << "(" << (unsigned int)c << ")"
                << "\n";
      continue;
    }

    FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 face->glyph->bitmap.buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    characters[c] = {
        texture,
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        face->glyph->advance.x,
    };
  }

  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GLFWwindow* init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "Main", NULL, NULL);

  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    exit(EXIT_FAILURE);
  }

  int nrAttributes;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
  std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes
            << std::endl;

  glViewport(0, 0, 800, 600);
  glEnable(GL_DEPTH_TEST);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouseCallback);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  stbi_set_flip_vertically_on_load(true);

  initFonts();

  return window;
}

unsigned int loadTexture(const std::string& path) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, nrChannels;
  unsigned char* data =
      stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
  if (!data) {
    std::cout << "Failed to load texture " << path << std::endl;
    return EXIT_FAILURE;
  }

  assert(nrChannels >= 3);
  assert(nrChannels <= 4);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
               nrChannels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  return texture;
}

void renderText(Shader& s, unsigned int fontVAO, unsigned int fontVBO,
                const std::string& text, float x, float y, float scale,
                glm::vec3 color) {
  glDepthMask(GL_FALSE);  // Don't write into the depth buffer
  s.use();
  s.setUniform("textColor", color);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(fontVAO);

  for (char c : text) {
    if (!characters.count(c)) {
      c = '?';
    }
    Character ch = characters[c];
    float xpos = x + ch.bearing.x * scale;
    float ypos = y - (ch.size.y - ch.bearing.y) * scale;

    float w = ch.size.x * scale;
    float h = ch.size.y * scale;

    float vertices[6][4] = {
        {xpos, ypos + h, 0.0F, 0.0F}, {xpos, ypos, 0.0F, 1.0F},
        {xpos + w, ypos, 1.0F, 1.0F}, {xpos, ypos + h, 0.0F, 0.0F},
        {xpos + w, ypos, 1.0F, 1.0F}, {xpos + w, ypos + h, 1.0F, 0.0F},
    };

    glBindTexture(GL_TEXTURE_2D, ch.textureId);
    glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    x += (ch.advance >> 6) * scale;
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDepthMask(GL_TRUE);  // Don't write into the depth buffer
}

int main() {
  GLFWwindow* window = init();

  // clang-format off
  float vertices[] = {
     -0.5F, -0.5F, -0.5F,  0.0F, 0.0F,
      0.5F, -0.5F, -0.5F,  1.0F, 0.0F,
      0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
      0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
     -0.5F,  0.5F, -0.5F,  0.0F, 1.0F,
     -0.5F, -0.5F, -0.5F,  0.0F, 0.0F,

     -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
      0.5F, -0.5F,  0.5F,  1.0F, 0.0F,
      0.5F,  0.5F,  0.5F,  1.0F, 1.0F,
      0.5F,  0.5F,  0.5F,  1.0F, 1.0F,
     -0.5F,  0.5F,  0.5F,  0.0F, 1.0F,
     -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,

     -0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
     -0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
     -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
     -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
     -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
     -0.5F,  0.5F,  0.5F,  1.0F, 0.0F,

      0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
      0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
      0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
      0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
      0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
      0.5F,  0.5F,  0.5F,  1.0F, 0.0F,

     -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
      0.5F, -0.5F, -0.5F,  1.0F, 1.0F,
      0.5F, -0.5F,  0.5F,  1.0F, 0.0F,
      0.5F, -0.5F,  0.5F,  1.0F, 0.0F,
     -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
     -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,

     -0.5F,  0.5F, -0.5F,  0.0F, 1.0F,
      0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
      0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
      0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
     -0.5F,  0.5F,  0.5F,  0.0F, 0.0F,
     -0.5F,  0.5F, -0.5F,  0.0F, 1.0F
  };
  // clang-format on

  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
  //              GL_STATIC_DRAW);

  unsigned int texture1 = loadTexture("assets/container.jpg");
  unsigned int texture2 = loadTexture("assets/awesomeface.png");

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texture2);

  Shader shader("shaders/shader1.vs", "shaders/shader1.fs");
  shader.use();
  shader.setUniform("texture1", 0);
  shader.setUniform("texture2", 1);

  glm::mat4 projection =
      glm::perspective(glm::radians(45.0F), 800.0F / 600.0F, 0.1F, 100.0F);
  shader.setUniform("projection", projection);

  Shader fontShader("shaders/font.vs", "shaders/font.fs");
  fontShader.use();
  fontShader.setUniform("projection", glm::ortho(0.0F, 800.0F, 0.0F, 600.0F));

  unsigned int fontVAO;
  unsigned int fontVBO;
  glGenVertexArrays(1, &fontVAO);
  glGenBuffers(1, &fontVBO);
  glBindVertexArray(fontVAO);
  glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  std::vector<glm::vec3> cubePositions = {
      glm::vec3(0.0F, 0.0F, 0.0F),    glm::vec3(2.0F, 5.0F, -15.0F),
      glm::vec3(-1.5F, -2.2F, -2.5F), glm::vec3(-3.8F, -2.0F, -12.3F),
      glm::vec3(2.4F, -0.4F, -3.5F),  glm::vec3(-1.7F, 3.0F, -7.5F),
      glm::vec3(1.3F, -2.0F, -2.5F),  glm::vec3(1.5F, 2.0F, -2.5F),
      glm::vec3(1.5F, 0.2F, -1.5F),   glm::vec3(-1.3F, 1.0F, -1.5F),
  };

  float lastFrame = 0.0F;
  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glBindVertexArray(VAO);
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    shader.setUniform("view", view);

    for (unsigned int i = 0; i < 10; i++) {
      glm::mat4 model = glm::translate(glm::mat4(1.0F), cubePositions[i]);
      float angle = 20.0F * i;
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(0.5F, 1.0F, 0.0F));
      shader.setUniform("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    std::string fps = std::to_string((int)(1 / deltaTime));
    renderText(fontShader, fontVAO, fontVBO, "Fps: " + fps, 25.0F, 575.0F, 0.1F,
               glm::vec3(1.0, 1.0, 1.0));

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}
