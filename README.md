# 🔴 Raycast Engine DDA

A first-person raycasting engine built in C using SDL3, implementing the classic **DDA (Digital Differential Analysis)** algorithm — the same technique used by Wolfenstein 3D.

![C](https://img.shields.io/badge/language-C-blue) ![SDL3](https://img.shields.io/badge/library-SDL3-orange) ![License](https://img.shields.io/badge/license-MIT-green)

---

## What It Is

A minimal, from-scratch 3D renderer that casts rays from a first-person perspective across a 2D tile map and draws vertical wall slices to simulate a 3D environment — no OpenGL, no game engine, just math and pixels.

Features:
- DDA raycasting with correct perspective projection
- Distance-based wall shading with side-face darkening
- Smooth player movement and rotation via delta time
- Collision detection against the tile map
- Minimap overlay rendered in the top-left corner

---

## Controls

| Key | Action |
|-----|--------|
| `W` | Move forward |
| `S` | Move backward |
| `A` | Rotate left |
| `D` | Rotate right |

---

## Building

### Requirements

- A C compiler (`gcc` or `clang`)
- [SDL3](https://github.com/libsdl-org/SDL) installed on your system

### Linux / macOS

```bash
gcc main.c -o raycast -lSDL3 -lm
./raycast
```

### Windows (MinGW)

```bash
gcc main.c -o raycast.exe -lSDL3 -lm
raycast.exe
```

> Make sure SDL3 headers and libraries are on your include/lib paths. See the [SDL3 wiki](https://wiki.libsdl.org/SDL3/Installation) for installation instructions.

---

## How It Works

### The DDA Algorithm

For each vertical column of the screen, a ray is cast from the player's position in the direction corresponding to that column's camera angle. The DDA algorithm steps the ray through the tile grid efficiently — jumping exactly to the next horizontal or vertical grid boundary at each step — until it hits a wall.

```
Camera plane
    |
    |--- ray 0
    |--- ray 1
    |--- ray N  →  hits wall  →  compute distance  →  draw slice
```

### Wall Rendering

Once a wall is hit, the **perpendicular wall distance** is used (not the Euclidean distance, to avoid the fisheye effect) to compute the height of the wall slice:

```
sliceHeight = SCREEN_HEIGHT / perpendicularDistance
```

Walls facing the player's left/right (side == 1) are rendered at 70% brightness to give the scene a sense of depth and shape.

### Distance Fog

Wall brightness falls off with distance using:

```c
float intensity = 1.0f / (1.0f + pWD * pWD * 0.03f);
```

This simulates atmospheric fog without any additional render passes.

---

## Map Format

The map is a 20×20 grid of integers defined in `main.c`:

- `1` = wall
- `0` = open space

```c
int map[MAP_SIZE][MAP_SIZE] = {
    {1,1,1,1,1, ...},
    {1,0,0,0,0, ...},
    ...
};
```

To create your own map, edit the `map` array. The outer border should remain `1` (wall) to prevent the player from escaping bounds.

---

## Project Structure

```
.
└── main.c      # Everything: map, player, raycaster, minimap, game loop
```

---

## Configuration

Key constants at the top of `main.c`:

| Constant | Default | Description |
|----------|---------|-------------|
| `MAP_SIZE` | `20` | Grid dimensions (NxN) |
| `TILE_SIZE` | `64` | World units per tile |
| `SCREEN_WIDTH` | `1280` | Render width in pixels |
| `SCREEN_HEIGHT` | `960` | Render height in pixels |

Player properties are set in the `Player` struct initializer:

```c
Player player = {
    .x = 96.0f,        // Starting X position
    .y = 96.0f,        // Starting Y position
    .speed = 300.0f,   // Movement speed
    .rotSpeed = 3.0f   // Rotation speed (radians/sec)
};
```

---

## References

- [Lode's Raycasting Tutorial](https://lodev.org/cgtutor/raycasting.html) — the definitive resource on DDA raycasting
- [SDL3 Documentation](https://wiki.libsdl.org/SDL3/FrontPage)
- [Wolfenstein 3D source code](https://github.com/id-Software/wolf3d) — the OG

---

## License

MIT — do whatever you want with it.
