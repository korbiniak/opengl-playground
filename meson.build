project(
    'opengl_test',
    'cpp',
    version: '0.1.0',
    default_options: ['cpp_std=c++20', 'warning_level=3'],
)

add_project_arguments('-O3', '-march=native', language: 'cpp')

glm_proj = subproject('glm')
freetype2_proj = subproject('freetype2')

gl_dep = dependency('gl')
glew_dep = dependency('glew')
glfw_dep = dependency('glfw3')
glm_dep = glm_proj.get_variable('glm_dep')
freetype2_dep = dependency('freetype2')
cxxopts_dep = dependency('cxxopts')
magic_enum_dep = dependency('magic_enum')
cereal_dep = dependency('cereal')

subdir('shaders')
subdir('assets')
subdir('fonts')

stb_image_lib = static_library(
    'stb_image',
    'lib/stb_image/stb_image.cpp',
    'lib/stb_image/stb_image_write.cpp',
    include_directories: include_directories('lib/stb_image'),
)

stb_image_dep = declare_dependency(
    link_with: stb_image_lib,
    include_directories: include_directories('lib/stb_image'),
)

src_files = files(
    'src/application.cpp',
    'src/camera.cpp',
    'src/circular_motion_component.cpp',
    'src/font_atlas.cpp',
    'src/game_object.cpp',
    'src/logger.cpp',
    'src/main2.cpp',
    'src/material.cpp',
    'src/mesh.cpp',
    'src/rainbow_component.cpp',
    'src/resource_manager.cpp',
    'src/scene.cpp',
    'src/shader.cpp',
    'src/text_mesh.cpp',
    'src/texture.cpp',
    'src/texture2d.cpp',
    'src/uitext.cpp',
    'src/utils.cpp',
    'src/rotation_component.cpp',
)

inc_dirs = include_directories('src')

executable(
    'main',
    src_files,
    dependencies: [
        cxxopts_dep,
        gl_dep,
        glew_dep,
        glfw_dep,
        glm_dep,
        freetype2_dep,
        magic_enum_dep,
        stb_image_dep,
        cereal_dep,
    ],
    install: false,
)

