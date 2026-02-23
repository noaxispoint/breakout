@echo off
rem =============================================================================
rem setup.bat – Windows setup script for the Breakout project
rem =============================================================================
rem
rem What this script does
rem ---------------------
rem   1. Creates the assets\ directory if it does not already exist.
rem   2. Downloads the DejaVu fonts release archive from GitHub and extracts
rem      DejaVuSans.ttf into assets\.  The font is used for all in-game text.
rem
rem Note: The TTF files are distributed as release artifacts (not committed to
rem the git repository), so we must download the .tar.bz2 release archive and
rem extract the font from it.  tar.exe (bundled with Windows 10 1803+) handles
rem bzip2 archives natively via the bundled libarchive.
rem
rem SFML 2.6.1 does NOT need to be installed manually.  CMake FetchContent
rem downloads and compiles it from source automatically during the first build.
rem An internet connection is required for that first build only.
rem
rem Prerequisites
rem -------------
rem   - Windows 10 version 1803 or later (provides tar.exe)
rem   - PowerShell 5+ (included in Windows 10 / 11)
rem   - CMake 3.16+  (https://cmake.org/download/)
rem   - Visual Studio 2019 or 2022 with "Desktop development with C++" workload
rem   - Git for Windows (https://git-scm.com/download/win)
rem
rem Usage
rem -----
rem   Double-click setup.bat, or run it from any Command Prompt / PowerShell.
rem
rem After the script completes, build and run (Visual Studio 2022 example):
rem   cmake -B build -G "Visual Studio 17 2022" -A x64
rem   cmake --build build --config Release
rem   build\Release\Breakout.exe
rem =============================================================================

setlocal EnableDelayedExpansion

set "ARCHIVE_URL=https://github.com/dejavu-fonts/dejavu-fonts/releases/download/version_2_37/dejavu-fonts-ttf-2.37.tar.bz2"
set "FONT_DEST=assets\DejaVuSans.ttf"

rem -----------------------------------------------------------------------------
rem Create assets directory
rem -----------------------------------------------------------------------------
echo ==^> Creating assets\ directory...
if not exist assets mkdir assets

rem -----------------------------------------------------------------------------
rem Download and extract font (skip if already present)
rem -----------------------------------------------------------------------------
if exist "%FONT_DEST%" (
    echo ==^> Font already present at %FONT_DEST% -- skipping download.
    goto :build_instructions
)

rem Build a unique temp directory path.
set "TMP_DIR=%TEMP%\breakout_dejavu_%RANDOM%"
mkdir "%TMP_DIR%"

echo ==^> Downloading DejaVu fonts release archive...
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
    "Invoke-WebRequest -Uri '%ARCHIVE_URL%' -OutFile '%TMP_DIR%\dejavu.tar.bz2' -UseBasicParsing"

if errorlevel 1 (
    echo.
    echo ERROR: Archive download failed. Check your internet connection.
    echo        Alternatively, place any TTF font at: %FONT_DEST%
    rd /s /q "%TMP_DIR%" 2>nul
    pause
    exit /b 1
)

echo ==^> Extracting DejaVuSans.ttf from archive...
rem tar.exe is included with Windows 10 1803+ and supports bzip2 via libarchive.
tar -xjf "%TMP_DIR%\dejavu.tar.bz2" -C "%TMP_DIR%"

if errorlevel 1 (
    echo.
    echo ERROR: Archive extraction failed.
    echo        Your Windows version may be too old (requires Windows 10 1803+).
    echo        Alternatively, place any TTF font at: %FONT_DEST%
    rd /s /q "%TMP_DIR%" 2>nul
    pause
    exit /b 1
)

rem Use PowerShell to search for the font anywhere in the extracted tree.
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
    "$f = Get-ChildItem -Path '%TMP_DIR%' -Filter 'DejaVuSans.ttf' -Recurse | Select-Object -First 1; if ($f) { Copy-Item $f.FullName -Destination '%FONT_DEST%'; Write-Host '==^> Font installed at %FONT_DEST%' } else { Write-Error 'DejaVuSans.ttf not found in archive'; exit 1 }"

if errorlevel 1 (
    echo.
    echo ERROR: Could not locate DejaVuSans.ttf inside the archive.
    echo        Please place any TTF font at: %FONT_DEST%
    rd /s /q "%TMP_DIR%" 2>nul
    pause
    exit /b 1
)

rem Clean up temp files.
rd /s /q "%TMP_DIR%" 2>nul

:build_instructions
echo.
echo ==^> Setup complete!  SFML will be fetched automatically during the build.
echo.
echo     Build commands (Visual Studio 2022):
echo       cmake -B build -G "Visual Studio 17 2022" -A x64
echo       cmake --build build --config Release
echo.
echo     Build commands (Visual Studio 2019):
echo       cmake -B build -G "Visual Studio 16 2019" -A x64
echo       cmake --build build --config Release
echo.
echo     Run:
echo       build\Release\Breakout.exe
echo.
pause
