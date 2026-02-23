/**
 * @file main.cpp
 * @brief Application entry point for the Breakout clone.
 *
 * Resolves the font path relative to the executable, constructs the Game
 * object, and hands control to Game::run() for the duration of the session.
 *
 * Font location
 * -------------
 * The setup script (setup.sh / setup.bat) downloads DejaVuSans.ttf into the
 * assets/ directory.  CMake's POST_BUILD step copies assets/ alongside the
 * binary so the relative path "assets/DejaVuSans.ttf" always resolves
 * correctly regardless of the working directory the user launches from,
 * provided the executable directory is used as the working directory.
 *
 * If the font is missing, Game's constructor emits a descriptive error message
 * to stderr and closes the window gracefully.
 */

#include "Game.hpp"

int main()
{
    // Path to the UI font, relative to the executable.
    // The setup script places DejaVuSans.ttf here; swap this path if you
    // prefer a different font.
    const std::string fontPath = "assets/DejaVuSans.ttf";

    Game game(fontPath);
    game.run();

    return 0;
}
