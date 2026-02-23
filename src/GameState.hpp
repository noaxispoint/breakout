/**
 * @file GameState.hpp
 * @brief Defines every distinct logical state the game can occupy.
 *
 * The Game class maintains one active GameState at a time and drives all
 * rendering and update logic through it.  Transitions between states are
 * triggered by player input (keyboard events) and in-game conditions
 * (ball lost, all bricks cleared, timer expiry).
 */

#pragma once

/**
 * @brief Enumeration of all top-level game states.
 */
enum class GameState
{
    /// Title screen displayed at startup.  Player presses Space to begin.
    MainMenu,

    /// Ball is resting on the paddle, waiting for the player to press Space.
    /// The paddle is still moveable so the player can aim the launch angle.
    BallOnPaddle,

    /// Normal play: ball is in motion, physics and collisions are active.
    Playing,

    /// Game is temporarily paused; no physics run.  Press P to resume.
    Paused,

    /// All bricks have been cleared.  A brief countdown runs before the next
    /// level is loaded.
    LevelComplete,

    /// The player has exhausted all lives.  Press Space to restart.
    GameOver,

    /// The player has cleared every level.  Press Space to play again.
    Victory,

    /// Full-screen controls reference card, reachable by pressing H from the
    /// main menu or the pause screen.  Press H or Esc to return.
    Controls
};
