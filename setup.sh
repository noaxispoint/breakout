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
# Done
# -----------------------------------------------------------------------------
echo ""
echo "==> Setup complete!  SFML will be fetched automatically during the build."
echo ""
echo "    Build commands:"
echo "      cmake -B build -DCMAKE_BUILD_TYPE=Release"
echo "      cmake --build build -j\$(sysctl -n hw.logicalcpu 2>/dev/null || nproc)"
echo ""
echo "    Run:"
echo "      ./build/Breakout"
echo ""
