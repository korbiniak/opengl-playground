project('opengl_test', 'cpp',
  version: '0.1.0',
  default_options: ['cpp_std=gnu++17', 'warning_level=3'])

add_project_arguments('-O3', '-march=native', language: 'cpp')

glm_proj = subproject('glm')
freetype2_proj = subproject('freetype2')

gl_dep = dependency('gl')
glew_dep = dependency('glew')
glfw_dep = dependency('glfw3')
glm_dep = glm_proj.get_variable('glm_dep')
freetype2_dep = dependency('freetype2')

subdir('shaders')
subdir('assets')
subdir('fonts')

stb_image_lib = static_library(
  'stb_image',
  'lib/stb_image/stb_image.cpp',
  include_directories: include_directories('lib/stb_image')
)

stb_image_dep = declare_dependency(
  link_with: stb_image_lib,
  include_directories: include_directories('lib/stb_image')
)

src_files = files(
  'src/main.cpp',
  'src/shader.cpp',
  'src/stb_image.cpp',
  'src/camera.cpp',
)

inc_dirs = include_directories('src')

executable('main',
           src_files,
  dependencies: [gl_dep, glew_dep, glfw_dep, glm_dep, freetype2_dep, stb_image_dep],
  install: false)

