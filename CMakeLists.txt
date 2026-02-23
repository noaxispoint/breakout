# =============================================================================
# CMakeLists.txt – Breakout clone build configuration
# =============================================================================
#
# Minimum version: 3.16 (FetchContent is stable from this version onward).
# CMake 3.21+ also enables automatic DLL-copy on Windows via
# TARGET_RUNTIME_DLLS; older versions require manual DLL placement (see README).
#
# SFML 2.6.1 is downloaded and compiled from source automatically during the
# first CMake configure step via FetchContent.  An internet connection is
# required for the initial build only; subsequent builds use the cached source.
#
# Usage (all platforms):
#   cmake -B build -DCMAKE_BUILD_TYPE=Release
#   cmake --build build
# =============================================================================

cmake_minimum_required(VERSION 3.16)

project(Breakout
    VERSION     1.0.0
    DESCRIPTION "A Breakout clone built with C++17 and SFML 2.6.1"
    LANGUAGES   CXX
)

# -----------------------------------------------------------------------------
# Language standard
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)   # Avoid non-portable GNU extensions.

# -----------------------------------------------------------------------------
# Fetch SFML 2.6.1 from GitHub
# -----------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
    GIT_SHALLOW    TRUE   # Download only the tagged commit, not full history.
)

# Disable SFML components the game does not use to keep build times shorter.
set(SFML_BUILD_AUDIO   OFF CACHE BOOL "Build SFML audio module"   FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "Build SFML network module" FORCE)

# -----------------------------------------------------------------------------
# Linux: display-backend selection
# -----------------------------------------------------------------------------
# SFML 2.6.x uses X11 on Linux by default.  The resulting binary runs on
# native X11 sessions AND on Wayland desktops via XWayland – the X11
# compatibility layer included with every major Wayland compositor (KDE
# Plasma, GNOME, sway, Hyprland, …).  No extra flags are needed for
# day-to-day Wayland desktop use.
#
# An experimental DRM/EGL backend is also available for embedded targets or
# bare Wayland TTY sessions that have no XWayland.  It bypasses the Wayland
# compositor protocol and writes directly to the display via KMS/DRM.
# Enable it with:
#   cmake -B build -DBREAKOUT_USE_DRM=ON
if(UNIX AND NOT APPLE)
    option(BREAKOUT_USE_DRM
        "Build SFML with the DRM/EGL backend instead of X11 (embedded / bare Wayland TTY)"
        OFF
    )
    if(BREAKOUT_USE_DRM)
        set(SFML_USE_DRM ON CACHE BOOL "Use DRM/EGL backend in SFML" FORCE)
        message(STATUS "Breakout: DRM/EGL backend enabled (SFML_USE_DRM=ON)")
    else()
        message(STATUS "Breakout: X11 backend (default). Wayland desktops use XWayland.")
    endif()
endif()

FetchContent_MakeAvailable(SFML)

# -----------------------------------------------------------------------------
# Executable target
# -----------------------------------------------------------------------------
set(BREAKOUT_SOURCES
    src/main.cpp
    src/Game.cpp
    src/Ball.cpp
    src/Paddle.cpp
    src/Brick.cpp
)

add_executable(Breakout ${BREAKOUT_SOURCES})

# Expose src/ for angle-bracket includes inside the project.
target_include_directories(Breakout PRIVATE src)

# Link against the three SFML modules the game uses.
target_link_libraries(Breakout
    PRIVATE
        sfml-graphics   # sf::RenderWindow, shapes, text, font.
        sfml-window     # Keyboard, events, window management.
        sfml-system     # sf::Clock, sf::Vector2, sf::Color, etc.
)

# Enable warnings on major compilers to catch common mistakes early.
if(MSVC)
    target_compile_options(Breakout PRIVATE /W4)
else()
    target_compile_options(Breakout PRIVATE -Wall -Wextra -Wpedantic)
endif()

# -----------------------------------------------------------------------------
# Copy assets/ directory alongside the binary after every build
# -----------------------------------------------------------------------------
# This ensures that assets/DejaVuSans.ttf (downloaded by the setup script)
# is always available next to the executable, regardless of the build tree.
add_custom_command(
    TARGET  Breakout POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:Breakout>/assets"
    COMMENT "Copying assets to output directory"
)

# -----------------------------------------------------------------------------
# Windows: copy SFML runtime DLLs alongside the executable
# -----------------------------------------------------------------------------
# CMake 3.21 introduced the TARGET_RUNTIME_DLLS generator expression, which
# enumerates every shared-library dependency of a target.  For older CMake
# versions, users must copy the DLLs manually (see README.md).
if(WIN32 AND CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
    add_custom_command(
        TARGET  Breakout POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:Breakout>
                $<TARGET_FILE_DIR:Breakout>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying SFML runtime DLLs to output directory"
    )
endif()

# -----------------------------------------------------------------------------
# Install rules (optional – useful when packaging a release)
# -----------------------------------------------------------------------------
install(TARGETS   Breakout RUNTIME DESTINATION bin)
install(DIRECTORY assets    DESTINATION bin)
