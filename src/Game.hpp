/**
 * @file Game.hpp
 * @brief Declaration of the Game class — the central game controller.
 *
 * Game owns every subsystem: the SFML window, game objects (Ball, Paddle,
 * Bricks), HUD text, and the game-state machine.  The public interface is a
 * single method, run(), which drives the main loop until the window is closed.
 *
 * Internal design
 * ---------------
 * The main loop (run()) delegates each frame to three private methods:
 *   1. processEvents()  – drains the SFML event queue; handles keyboard input
 *                         that drives state transitions.
 *   2. update()         – advances physics, paddle movement, and collision
 *                         detection; evaluates win/loss conditions.
 *   3. render()         – clears the back buffer and draws all visible objects,
 *                         then presents the finished frame.
 *
 * Collision detection
 * -------------------
 * Ball vs. walls, paddle, and bricks are resolved in separate helper methods.
 * Brick collisions use the circle–AABB nearest-point algorithm to produce a
 * physically plausible reflection normal.  Only the first brick collision is
 * resolved per frame to avoid double-reflections at brick corners.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "GameState.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"
#include "Brick.hpp"

/**
 * @brief Top-level game controller for the Breakout clone.
 *
 * Instantiate exactly one Game object, then call run() to enter the event
 * loop.  The constructor opens the window and loads required assets; if an
 * asset fails to load the window is closed immediately and run() returns
 * without error.
 */
class Game
{
public:
    /**
     * @brief Constructs the Game and initialises all subsystems.
     *
     * Actions performed during construction:
     *   - Opens the sf::RenderWindow at the size defined in Constants.
     *   - Seeds the random-number generator.
     *   - Loads the font from @p fontPath; terminates the window on failure.
     *   - Creates the initial brick grid.
     *   - Positions the ball on the paddle (BallOnPaddle state).
     *
     * @param fontPath  Filesystem path to the TTF/OTF font file used for
     *                  all HUD and overlay text.
     */
    explicit Game(const std::string& fontPath);

    /**
     * @brief Runs the main game loop until the window is closed.
     *
     * Each iteration:
     *   1. Measures the frame delta time (capped at 50 ms to prevent
     *      physics explosions after focus loss or debugger pauses).
     *   2. Calls processEvents(), then update(), then render().
     */
    void run();

private:
    // =========================================================================
    // Initialisation helpers
    // =========================================================================

    /**
     * @brief Populates the brick grid for the current level.
     *
     * Bricks are arranged in BRICK_ROWS × BRICK_COLS, centred horizontally.
     * Rows closer to the top of the screen are worth more points.  On levels
     * beyond the first, extra hit points are added to every brick row.
     */
    void createBricks();

    /**
     * @brief Places the ball on the paddle and enters BallOnPaddle state.
     *
     * The ball centre is set just above the paddle's top edge so it appears
     * to rest on the paddle surface until the player presses Space.
     */
    void resetBallOnPaddle();

    /**
     * @brief Resets all game state and starts from level 1.
     *
     * Resets score, lives, level, and ball speed; re-centres the paddle;
     * rebuilds the brick grid; and calls resetBallOnPaddle().
     */
    void restartGame();

    /**
     * @brief Advances to the next level after the current one is cleared.
     *
     * Increments the level counter, increases ball speed (capped at
     * BALL_MAX_SPEED), rebuilds the brick grid, and resets the ball on paddle.
     */
    void advanceLevel();

    // =========================================================================
    // Main loop steps
    // =========================================================================

    /**
     * @brief Drains the SFML event queue and handles relevant events.
     *
     * Handles:
     *   - sf::Event::Closed  → closes the window.
     *   - Escape             → closes the window.
     *   - Space              → starts, launches, or restarts depending on state.
     *   - P                  → toggles pause while Playing or Paused.
     */
    void processEvents();

    /**
     * @brief Advances game logic by @p deltaTime seconds.
     *
     * - Moves the paddle according to live keyboard state.
     * - In BallOnPaddle state: keeps the ball anchored to the paddle centre.
     * - In Playing state: moves the ball, resolves all collisions, checks for
     *   ball-lost and level-complete conditions.
     *
     * @param deltaTime  Elapsed time since the previous frame, in seconds.
     */
    void update(float deltaTime);

    /**
     * @brief Clears the window and draws all visible game elements.
     *
     * Draw order: background colour → bricks → paddle → ball → HUD → overlay.
     * The overlay is only drawn for non-playing states (menus, game-over, etc.).
     */
    void render();

    // =========================================================================
    // Collision helpers
    // =========================================================================

    /**
     * @brief Tests the ball against the four window walls and responds.
     *
     * - Left and right walls: reflect the ball horizontally.
     * - Top wall: reflect the ball vertically.
     * - Bottom boundary: deduct a life; either reset the ball or trigger
     *   GameOver if no lives remain.
     */
    void handleWallCollisions();

    /**
     * @brief Tests the ball against the paddle and responds if they intersect.
     *
     * Uses an AABB broad phase before computing the precise contact position.
     * Collision response maps the horizontal hit offset (in [-1, 1] relative
     * to the paddle centre) to a launch angle of up to ±75° from vertical,
     * giving the player meaningful directional control.
     *
     * The ball is nudged above the paddle surface after each collision to
     * prevent it from becoming trapped inside the shape.
     */
    void handlePaddleCollision();

    /**
     * @brief Tests the ball against every active brick and responds.
     *
     * Uses the circle–AABB nearest-point method to find the collision normal.
     * Only the first intersection resolved per frame reverses the ball's
     * direction; subsequent bricks hit in the same frame are still damaged but
     * do not cause additional reflections, preventing erratic multi-bounce
     * behaviour at brick-cluster boundaries.
     */
    void handleBrickCollisions();

    /**
     * @brief Reflects the ball's velocity off a surface defined by @p normal.
     *
     * Applies the standard specular-reflection formula:
     *   r = v − 2(v·n)n
     *
     * @param normal  Unit vector perpendicular to the reflecting surface,
     *                pointing away from the surface into the ball's half-space.
     */
    void reflectBall(sf::Vector2f normal);

    // =========================================================================
    // Render helpers
    // =========================================================================

    /**
     * @brief Draws the heads-up display: score (left), level (centre),
     *        and life indicators (right).
     */
    void drawHUD();

    /**
     * @brief Draws a semi-transparent overlay appropriate for the current state.
     *
     * Used for all non-Playing states: MainMenu, Paused, LevelComplete,
     * GameOver, and Victory.  Each state gets a dark backdrop plus a set of
     * centred text strings with game-specific messaging.
     */
    void drawStateOverlay();

    /**
     * @brief Draws the full-screen controls reference card.
     *
     * Shows every keyboard shortcut and the scoring table with colour-coded
     * brick rows.  Triggered by pressing H from the main menu or pause screen;
     * returns to the previous state when the player presses H or Esc.
     *
     * Layout
     * ------
     * The screen is divided into three sections separated by faint horizontal
     * rules: Movement, Game Controls, and Scoring.  Key labels are drawn in a
     * fixed left column; descriptions in a fixed right column.  Brick-row
     * score entries include a small filled circle in the matching brick colour.
     */
    void drawControlsScreen();

    /**
     * @brief Horizontally centres an sf::Text object within the window.
     *
     * Sets the text's X position so its bounding box is centred, and its Y
     * position to @p y.
     *
     * @param text  Text object to reposition (string must already be set).
     * @param y     Desired Y coordinate of the top of the text, in pixels.
     */
    void centreTextHorizontally(sf::Text& text, float y);

    /**
     * @brief Creates a configured sf::Text ready for rendering.
     *
     * Convenience factory that assigns the shared font, a character size, and
     * a fill colour in a single call.
     *
     * @param content        The string to display.
     * @param characterSize  Point size passed to sf::Text::setCharacterSize.
     * @param color          Fill colour of the text glyphs.
     * @return sf::Text  Fully initialised text object.
     */
    sf::Text makeText(const std::string& content,
                      unsigned int characterSize,
                      sf::Color color) const;

    // =========================================================================
    // Member data
    // =========================================================================

    sf::RenderWindow   window;              ///< SFML OS window / OpenGL context.
    sf::Font           font;               ///< Shared font for all text rendering.
    sf::Clock          clock;              ///< Measures per-frame delta time.

    Ball               ball;              ///< The bouncing ball.
    Paddle             paddle;            ///< Player-controlled paddle.
    std::vector<Brick> bricks;            ///< All bricks in the current level.

    GameState          state;             ///< Current logical game state.
    int                score;             ///< Accumulated player score.
    int                lives;             ///< Remaining player lives.
    int                level;             ///< Current level number (1-based).
    float              ballSpeed;         ///< Active ball speed in pixels/second.

    float              levelCompleteTimer;///< Countdown (seconds) before advancing.
    int                bricksRemaining;   ///< Live brick count in current level.

    /// State to return to when the player closes the Controls screen.
    /// Set to MainMenu when H is pressed from the main menu, Paused when
    /// pressed while the game is paused.
    GameState          previousState;
};
