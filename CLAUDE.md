# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

This project uses Meson build system with Ninja backend.

**Build commands:**
```bash
# Initial setup (if not already configured)
meson setup build

# Compile the project
meson compile -C build

# Run the executable
./build/main

# Run with custom options
./build/main --width 1920 --height 1080 --log-level DEBUG
```

**Available command-line options:**
- `--width`: Window width (default: 800)
- `--height`: Window height (default: 600)
- `--log-level` or `-l`: Log level - possible values: DEBUG, INFO, WARNING, ERROR (default: INFO)
- `--help` or `-h`: Print usage

## Architecture Overview

This is an OpenGL-based game engine using modern C++20. The architecture follows an Entity-Component-System (ECS) inspired pattern with a scene graph.

### Core Architecture

**Application → Scene → GameObjects → Components**

- **Application** (`src/application.h`): Main application controller that manages GLFW window, input handling, and the main render loop. It owns the `Scene`, `UI`, and `ResourceManager`.

- **Scene** (`src/scene.h`): Container for all game objects and cameras. Handles:
  - Update loop delegation to all objects
  - Render pass with material batching (groups objects by material to minimize state changes)
  - Light collection and uniform binding
  - Camera management

- **GameObject** (`src/game_object.h`): Basic scene entity with transform (position, rotation, scale using quaternions), mesh, material, and components. Uses a dirty flag pattern for efficient matrix updates.

- **Component** (`src/component.h`): Base class for attachable behaviors. Components have access to their parent GameObject and implement an `update()` method called each frame.

### Component System

Components extend GameObject functionality without inheritance bloat. Current implementations:

- **LightComponent** (`src/light_component.h`): Abstract base for all light types. Implements a uniform naming convention for shader binding.
  - **DirectionalLightComponent**: Infinite distance lights with direction
  - **PointLightComponent**: Position-based lights with attenuation
  - **SpotlightComponent**: Cone-shaped lights with direction, position, and cutoff angles

- **CircularMotionComponent**: Example component for circular motion around a center point

**Adding new components:**
1. Inherit from `Component`
2. Implement `update(float deltaTime)`
3. Override `onAttach(GameObject*)` if initialization is needed
4. Attach to GameObjects using `gameObject->addComponent<T>(std::make_unique<T>())`
5. Query components using `gameObject->getComponents<T>()`

### Lighting System

The lighting system supports multiple light types simultaneously. Scene::render() collects all enabled LightComponent instances and binds them to shaders:

- Maximum 8 lights per type (directional, point, spotlight)
- Shaders receive uniform arrays: `dirLights[]`, `pointLights[]`, `spotLights[]`
- Light count uniforms: `numDirLights`, `numPointLights`, `numSpotLights`
- Only shaders with lighting uniforms receive light data (checked via `hasUniform()`)

**Light uniforms structure** (example for point lights):
```glsl
pointLights[i].position
pointLights[i].ambient
pointLights[i].diffuse
pointLights[i].specular
pointLights[i].constant
pointLights[i].linear
pointLights[i].quadratic
```

### Material System

**Material** (`src/material.h`): Encapsulates shader and texture bindings. Materials contain:
- Shader reference
- Diffuse texture (optional)
- Specular map (optional)
- Shininess value
- Base color

The Scene groups objects by material before rendering to minimize GPU state changes.

### Resource Management

**ResourceManager** (`src/resource_manager.h`): Centralized resource loading and caching using `std::shared_ptr` for automatic memory management. Caches:
- Shaders (vertex + fragment)
- Meshes (vertices + indices)
- Textures (via stb_image)
- Font atlases (via FreeType)

Resources are loaded once and reused via shared pointers. Access pattern:
```cpp
auto shader = resourceManager.loadShader("name", "path.vert", "path.frag");
auto mesh = resourceManager.loadMesh("name", vertices, indices);
auto texture = resourceManager.loadTexture("name", "path.png");
```

### Rendering Pipeline

1. **Update Phase**: Scene updates all GameObjects and their components with deltaTime
2. **Material Batching**: Scene groups objects by material to reduce state changes
3. **Render Phase**: For each material:
   - Bind shader
   - Set lighting uniforms (if shader supports lighting)
   - Set view/projection matrices
   - For each object with that material:
     - Bind material (textures, properties)
     - Set model matrix
     - Draw geometry

### Camera System

Camera uses view and projection matrices. The active camera is selected from the Scene's camera list. Cameras update their matrices each frame and provide them to shaders for rendering.

## Dependencies

All dependencies are managed via Meson subprojects or system packages:

- **OpenGL** (gl): Graphics API
- **GLEW** (glew): OpenGL extension wrangler
- **GLFW3** (glfw3): Window and input handling
- **GLM** (glm): Math library for graphics (subproject)
- **FreeType2** (freetype2): Font rendering (subproject)
- **cxxopts**: Command-line argument parsing (subproject)
- **magic_enum**: Compile-time enum reflection (subproject)
- **cereal**: Serialization library (subproject)
- **stb_image**: Image loading (vendored in `lib/stb_image/`)

## Project Structure

- `src/`: C++ source and header files
- `shaders/`: GLSL vertex and fragment shaders
- `assets/`: Runtime assets (textures, models)
- `fonts/`: Font files for text rendering
- `lib/stb_image/`: Vendored stb_image library
- `subprojects/`: Meson subproject dependencies
- `build/`: Build output directory (executable: `build/main`)

## Code Style

The project uses `.clangformat` for consistent code formatting. Key conventions:
- C++20 features are used (concepts, ranges, etc. where applicable)
- Header guards follow `#ifndef FILENAME_H` pattern
- Smart pointers (`std::shared_ptr`, `std::unique_ptr`) for memory management
- Const correctness is enforced
- GLM for all vector/matrix math

## Logging

Custom logging system in `src/logger.h` with macros:
- `LOG_DEBUG()`: Debug-level messages
- `LOG_INFO()`: Informational messages
- `LOG_WARNING()`: Warning messages
- `LOG_ERROR()`: Error messages

Log level is controlled via command-line `--log-level` flag.
