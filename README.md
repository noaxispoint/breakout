# Breakout

A classic Breakout arcade clone written in **C++17** using **SFML 2.6.1** for
windowing, rendering, and input.

---

## Gameplay

- Clear all 60 bricks per level to advance.
- 5 levels total – ball speed increases with each level.
- 3 lives – the ball resets onto the paddle after each miss.
- Paddle deflection: where you strike the ball along the paddle controls the
  rebound angle (edge = sharp angle, centre = steep upward launch).
- Brick colour darkens with each hit, indicating remaining durability.

### Scoring

| Row (top → bottom) | Points per brick (Level 1) |
|--------------------|---------------------------|
| Red                | 60                         |
| Orange             | 50                         |
| Yellow             | 40                         |
| Green              | 30                         |
| Blue               | 20                         |
| Purple             | 10                         |

Multi-hit bricks (higher levels) scale point values with hit-point count.

---

## Controls

| Key              | Action                         |
|------------------|--------------------------------|
| `←` / `A`        | Move paddle left               |
| `→` / `D`        | Move paddle right              |
| `Space`          | Launch ball / Start / Restart  |
| `P`              | Pause / Resume                 |
| `Esc`            | Quit                           |

---

## Building on macOS

### Prerequisites

- macOS 10.15 Catalina or later
- Xcode Command Line Tools (`xcode-select --install`)
- [Homebrew](https://brew.sh) (recommended)
- CMake 3.16+ — `brew install cmake`
- Git — `brew install git`

### 1 – Run the setup script

```bash
chmod +x setup.sh && ./setup.sh
```

This downloads the DejaVu fonts release archive from GitHub, extracts
`DejaVuSans.ttf`, and places it in `assets/`.  SFML itself is fetched
automatically by CMake during the first build.

### 2 – Configure and build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(sysctl -n hw.logicalcpu)
```

### 3 – Run

```bash
./build/Breakout
```

> **Note:** The working directory must be the project root (or any directory
> from which `assets/DejaVuSans.ttf` is reachable) when launching the
> executable, **or** launch directly from the `build/` directory since CMake
> copies `assets/` there automatically.

---

## Building on Windows

### Prerequisites

- Windows 10 or 11
- [Visual Studio 2019 or 2022](https://visualstudio.microsoft.com/) with the
  **Desktop development with C++** workload installed
  *or* LLVM/Clang + [Ninja](https://ninja-build.org/)
- [CMake 3.16+](https://cmake.org/download/) (tick "Add CMake to PATH" during install)
- [Git for Windows](https://git-scm.com/download/win)

### 1 – Run the setup script

Open **Command Prompt** or **PowerShell** in the project directory and run:

```bat
setup.bat
```

Or double-click `setup.bat` in File Explorer.

### 2 – Configure and build (Visual Studio 2022)

```bat
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

For Visual Studio 2019 replace `"Visual Studio 17 2022"` with
`"Visual Studio 16 2019"`.

### 3 – Copy DLLs (if needed)

CMake 3.21+ copies SFML DLLs alongside the executable automatically.  If you
are using an older CMake version, manually copy all `.dll` files from
`build\_deps\sfml-build\lib\Release\` into `build\Release\`.

### 4 – Run

```bat
build\Release\Breakout.exe
```

---

## Project structure

```
breakout/
├── CMakeLists.txt           Build system (downloads SFML via FetchContent)
├── README.md                This file
├── CREDITS.md               Third-party attributions
├── LICENSE                  MIT licence for game source code
├── setup.sh                 macOS / Linux setup helper
├── setup.bat                Windows setup helper
├── assets/
│   └── DejaVuSans.ttf       Font – downloaded by setup script
└── src/
    ├── main.cpp             Entry point
    ├── constants.hpp        Global compile-time constants
    ├── GameState.hpp        Game-state enumeration
    ├── Ball.hpp / .cpp      Ball entity
    ├── Paddle.hpp / .cpp    Player paddle entity
    ├── Brick.hpp / .cpp     Brick entity
    └── Game.hpp / .cpp      Central game controller
```

---

## Dependencies

| Library     | Version | Licence        | Obtained via        |
|-------------|---------|----------------|---------------------|
| SFML        | 2.6.1   | zlib/png       | CMake FetchContent  |
| DejaVu Sans | 2.37    | Bitstream Vera | setup.sh / setup.bat|

See [CREDITS.md](CREDITS.md) for full attribution details.

---

## Licence

The Breakout game source code in this repository is released under the
[MIT Licence](LICENSE).
