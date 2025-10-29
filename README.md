# OpenGL Game Engine

A C++20 OpenGL-based 3D game engine with component-based architecture, multiple light types, and text rendering.

## Features

- OpenGL 3.3 Core Profile rendering
- Entity-Component-System architecture
- Multiple light types (directional, point, spotlight)
- Material system with texture support
- Resource management with caching
- Text rendering using FreeType
- First-person camera with mouse look

## Prerequisites

### System Dependencies

**Ubuntu/Debian:**
```bash
sudo apt install build-essential meson ninja-build libgl-dev libglew-dev libglfw3-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install gcc-c++ meson ninja-build mesa-libGL-devel glew-devel glfw-devel
```

**Arch Linux:**
```bash
sudo pacman -S base-devel meson ninja glew glfw-x11
```

**macOS (via Homebrew):**
```bash
brew install meson ninja glew glfw
```

Other dependencies (GLM, FreeType, cxxopts, magic_enum, cereal, stb_image) are managed automatically via Meson subprojects.

## Building

```bash
# Configure (first time only)
meson setup build

# Compile
meson compile -C build
```

The executable will be at `build/main`.

## Running

```bash
./build/main
```

### Command-line Options

```bash
./build/main [OPTIONS]
```

- `--width <pixels>` - Window width (default: 800)
- `--height <pixels>` - Window height (default: 600)
- `-l, --log-level <LEVEL>` - Log level: DEBUG, INFO, WARNING, ERROR (default: INFO)
- `-h, --help` - Display help

**Example:**
```bash
./build/main --width 1920 --height 1080 --log-level DEBUG
```

## Controls

### Camera Movement
- **W** - Move forward
- **S** - Move backward
- **A** - Strafe left
- **D** - Strafe right
- **Space** - Move up
- **Left Shift** - Move down
- **Mouse** - Look around

### Other
- **Tab** - Toggle wireframe mode
- **ESC** - Exit

## Development

### Adding Components

Create a component inheriting from `Component`:

```cpp
class MyComponent : public Component {
public:
    void update(float deltaTime) override {
        // Update logic here
    }
};
```

Attach to a GameObject:
```cpp
gameObject->addComponent<MyComponent>(std::make_unique<MyComponent>());
```

Query components:
```cpp
std::vector<MyComponent*> components = gameObject->getComponents<MyComponent>();
```

### Loading Resources

```cpp
std::shared_ptr<Shader> shader = resourceManager.loadShader("name", "path.vert", "path.frag");
std::shared_ptr<Mesh> mesh = resourceManager.loadMesh("name", vertices, indices);
std::shared_ptr<Texture2D> texture = resourceManager.loadTexture("name", "path.png");
```

For more architectural details, see [CLAUDE.md](CLAUDE.md).

## License

[Add your license information here]

## Acknowledgments

- [LearnOpenGL](https://learnopengl.com/) - OpenGL tutorials
- stb_image by Sean Barrett
