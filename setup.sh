#!/usr/bin/env bash
# =============================================================================
# setup.sh – macOS / Linux setup script for the Breakout project
# =============================================================================
#
# What this script does
# ---------------------
#   1. Creates the assets/ directory if it does not already exist.
#   2. Downloads the DejaVu fonts release archive from GitHub and extracts
#      DejaVuSans.ttf into assets/.  The font is used for all in-game text.
#
# Note: The TTF files are distributed as release artifacts (not committed to
# the git repository), so we must download the .tar.bz2 release archive and
# extract the font from it.
#
# SFML 2.6.1 does NOT need to be installed manually.  CMake FetchContent
# downloads and compiles it from source automatically during the first build.
# An internet connection is required for that first build only.
#
# Prerequisites
# -------------
#   - CMake 3.16 or newer
#   - A C++17 compiler (Clang or GCC)
#   - Git (used internally by CMake FetchContent)
#   - curl or wget  (to download the archive)
#   - tar with bzip2 support (standard on macOS and most Linux distros)
#
# Usage
# -----
#   chmod +x setup.sh
#   ./setup.sh
#
# After the script completes, build and run with:
#   cmake -B build -DCMAKE_BUILD_TYPE=Release
#   cmake --build build -j$(sysctl -n hw.logicalcpu 2>/dev/null || nproc)
#   ./build/Breakout
# =============================================================================

set -euo pipefail

# Official release archive from the DejaVu Fonts project on GitHub.
# The TTF files live inside the archive under dejavu-fonts-ttf-2.37/ttf/.
ARCHIVE_URL="https://github.com/dejavu-fonts/dejavu-fonts/releases/download/version_2_37/dejavu-fonts-ttf-2.37.tar.bz2"
FONT_DEST="assets/DejaVuSans.ttf"

# -----------------------------------------------------------------------------
# Create assets directory
# -----------------------------------------------------------------------------
echo "==> Creating assets/ directory..."
mkdir -p assets

# -----------------------------------------------------------------------------
# Download and extract font (skip if already present)
# -----------------------------------------------------------------------------
if [ -f "$FONT_DEST" ]; then
    echo "==> Font already present at $FONT_DEST – skipping download."
else
    # Create a temporary directory that is cleaned up on exit.
    TMP_DIR=$(mktemp -d)
    trap 'rm -rf "$TMP_DIR"' EXIT

    echo "==> Downloading DejaVu fonts release archive..."
    if command -v curl &>/dev/null; then
        curl -fsSL -o "$TMP_DIR/dejavu.tar.bz2" "$ARCHIVE_URL"
    elif command -v wget &>/dev/null; then
        wget -q -O "$TMP_DIR/dejavu.tar.bz2" "$ARCHIVE_URL"
    else
        echo ""
        echo "ERROR: Neither curl nor wget was found on your PATH."
        echo "       Please install one and re-run, or place any TTF font at:"
        echo "       $FONT_DEST"
        exit 1
    fi

    echo "==> Extracting DejaVuSans.ttf from archive..."
    tar -xjf "$TMP_DIR/dejavu.tar.bz2" -C "$TMP_DIR"

    # Locate the font inside the extracted tree (path may vary between releases).
    FONT_FILE=$(find "$TMP_DIR" -name "DejaVuSans.ttf" | head -1)

    if [ -z "$FONT_FILE" ]; then
        echo ""
        echo "ERROR: DejaVuSans.ttf was not found in the downloaded archive."
        echo "       Please place any TTF font at: $FONT_DEST"
        exit 1
    fi

    cp "$FONT_FILE" "$FONT_DEST"
    echo "==> Font installed at $FONT_DEST"
fi

# -----------------------------------------------------------------------------
# Linux: offer to install SFML system dependencies
# -----------------------------------------------------------------------------
# SFML 2.6.x builds from source but still needs X11 development headers and
# a few other system libraries.  The prompt below is skipped on macOS.
# -----------------------------------------------------------------------------
if [[ "$(uname -s)" == "Linux" ]]; then
    echo ""
    echo "==> Linux detected – SFML needs these system libraries to build:"

    # Returns 0 (true) if the user answers Y/y/Enter, 1 (false) otherwise.
    # Handles non-interactive stdin gracefully (defaults to Y on EOF).
    prompt_install() {
        local desc="$1"
        local pkgs="$2"
        echo "    Distribution : $desc"
        echo "    Packages     : $pkgs"
        local reply
        read -r -p "    Install now via sudo? [Y/n] " reply || reply="Y"
        reply="${reply:-Y}"
        [[ "$reply" =~ ^[Yy] ]]
    }

    if command -v apt-get &>/dev/null; then
        APT_PKGS="cmake g++ git libx11-dev libxrandr-dev libxcursor-dev libudev-dev libfreetype-dev libgl-dev"
        if prompt_install "Debian / Ubuntu / Mint" "$APT_PKGS"; then
            # shellcheck disable=SC2086
            sudo apt-get install -y $APT_PKGS
        else
            echo "    Skipped – install manually before running cmake."
        fi

    elif command -v dnf &>/dev/null; then
        DNF_PKGS="cmake gcc-c++ git libX11-devel libXrandr-devel libXcursor-devel systemd-devel mesa-libGL-devel freetype-devel"
        if prompt_install "Fedora / RHEL / CentOS Stream" "$DNF_PKGS"; then
            # shellcheck disable=SC2086
            sudo dnf install -y $DNF_PKGS
        else
            echo "    Skipped – install manually before running cmake."
        fi

    elif command -v pacman &>/dev/null; then
        PACMAN_PKGS="cmake gcc git libx11 libxrandr libxcursor systemd-libs mesa freetype2"
        if prompt_install "Arch / Manjaro" "$PACMAN_PKGS"; then
            # shellcheck disable=SC2086
            sudo pacman -S --needed $PACMAN_PKGS
        else
            echo "    Skipped – install manually before running cmake."
        fi

    elif command -v zypper &>/dev/null; then
        ZYPPER_PKGS="cmake gcc-c++ git libX11-devel libXrandr-devel libXcursor-devel libudev-devel Mesa-libGL-devel freetype2-devel"
        if prompt_install "openSUSE" "$ZYPPER_PKGS"; then
            # shellcheck disable=SC2086
            sudo zypper install -y $ZYPPER_PKGS
        else
            echo "    Skipped – install manually before running cmake."
        fi

    else
        echo "    Package manager not recognised."
        echo "    Install SFML build dependencies manually – see README.md for"
        echo "    the package names for your distribution."
    fi
fi

# -----------------------------------------------------------------------------
# Done
# -----------------------------------------------------------------------------
echo ""
echo "==> Setup complete!  SFML will be fetched automatically during the build."
echo ""
if [[ "$(uname -s)" == "Linux" ]]; then
    echo "    Build commands (Linux):"
    echo "      cmake -B build -DCMAKE_BUILD_TYPE=Release"
    echo "      cmake --build build -j\$(nproc)"
    echo ""
    echo "    Wayland note: the binary uses SFML's X11 backend and runs via"
    echo "    XWayland on all major Wayland desktops – no extra flags needed."
    echo "    For embedded/bare-TTY Wayland use: add -DBREAKOUT_USE_DRM=ON"
else
    echo "    Build commands:"
    echo "      cmake -B build -DCMAKE_BUILD_TYPE=Release"
    echo "      cmake --build build -j\$(sysctl -n hw.logicalcpu 2>/dev/null || nproc)"
fi
echo ""
echo "    Run:"
echo "      ./build/Breakout"
echo ""
