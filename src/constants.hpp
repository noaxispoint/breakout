/**
 * @file constants.hpp
 * @brief Global compile-time constants shared across all game components.
 *
 * Every tuneable value that affects gameplay, layout, or presentation lives
 * here so that adjustments require changes in exactly one place.
 */

#pragma once

#include <cstdint>

namespace Constants {

    // =========================================================================
    // Window
    // =========================================================================

    /// Width of the game window in pixels.
    constexpr uint32_t WINDOW_WIDTH  = 800;

    /// Height of the game window in pixels.
    constexpr uint32_t WINDOW_HEIGHT = 600;

    /// Target frames per second; passed to sf::Window::setFramerateLimit.
    constexpr uint32_t FRAME_RATE = 60;

    /// Text shown in the OS title bar.
    constexpr const char* WINDOW_TITLE = "Breakout";

    // =========================================================================
    // Paddle
    // =========================================================================

    /// Paddle width in pixels.
    constexpr float PADDLE_WIDTH  = 120.0f;

    /// Paddle height in pixels.
    constexpr float PADDLE_HEIGHT = 14.0f;

    /// Horizontal movement speed of the paddle in pixels per second.
    constexpr float PADDLE_SPEED  = 500.0f;

    /// Vertical distance from the bottom of the window to the top of the paddle.
    constexpr float PADDLE_Y_OFFSET = 45.0f;

    // =========================================================================
    // Ball
    // =========================================================================

    /// Radius of the ball circle in pixels.
    constexpr float BALL_RADIUS = 8.0f;

    /// Ball speed at the start of level 1, in pixels per second.
    constexpr float BALL_INITIAL_SPEED = 320.0f;

    /// Speed increase applied each time the player clears a level.
    constexpr float BALL_SPEED_STEP = 35.0f;

    /// Hard upper limit on ball speed to keep the game playable.
    constexpr float BALL_MAX_SPEED = 600.0f;

    // =========================================================================
    // Bricks
    // =========================================================================

    /// Number of brick rows in the grid.
    constexpr int BRICK_ROWS = 6;

    /// Number of brick columns in the grid.
    constexpr int BRICK_COLS = 10;

    /// Width of each brick in pixels.
    constexpr float BRICK_WIDTH = 68.0f;

    /// Height of each brick in pixels.
    constexpr float BRICK_HEIGHT = 22.0f;

    /// Horizontal and vertical gap between adjacent bricks in pixels.
    constexpr float BRICK_PADDING = 4.0f;

    /// Vertical distance from the top of the window to the first brick row.
    constexpr float BRICK_TOP_OFFSET = 60.0f;

    // =========================================================================
    // Game / Lives
    // =========================================================================

    /// Number of lives the player begins with.
    constexpr int INITIAL_LIVES = 3;

    /// Total number of levels; the Victory screen is shown after the last one.
    constexpr int MAX_LEVELS = 5;

    /// Seconds to display the "Level Complete" screen before advancing.
    constexpr float LEVEL_COMPLETE_DELAY = 2.0f;

    // =========================================================================
    // HUD / Typography
    // =========================================================================

    /// Font size for large headlines (title, game-over, victory).
    constexpr unsigned int FONT_SIZE_LARGE  = 40;

    /// Font size for score, level, and menu body text.
    constexpr unsigned int FONT_SIZE_MEDIUM = 24;

    /// Font size for small secondary hints (e.g. "Press SPACE to launch").
    constexpr unsigned int FONT_SIZE_SMALL  = 18;

    /// Radius of the small life-indicator circles drawn at the bottom of the screen.
    constexpr float LIFE_INDICATOR_RADIUS = 7.0f;

    /// Horizontal gap between consecutive life-indicator circles.
    constexpr float LIFE_INDICATOR_GAP = 4.0f;

} // namespace Constants
