/**
 * @file Game.cpp
 * @brief Implementation of the Game class.
 */

#include "Game.hpp"
#include "constants.hpp"

#include <SFML/Graphics.hpp>

#include <algorithm>  // std::min, std::max
#include <array>
#include <cmath>      // std::sqrt, std::sin, std::cos
#include <cstdlib>    // std::srand, std::rand
#include <ctime>      // std::time
#include <iostream>   // std::cerr
#include <sstream>    // std::ostringstream

// =============================================================================
// Brick layout data – one entry per row, top row first
// =============================================================================

/// Fill colours for each brick row.
static const std::array<sf::Color, Constants::BRICK_ROWS> ROW_COLORS = {{
    sf::Color(220,  45,  45),  // Row 0 – Red     (highest value)
    sf::Color(230, 120,  20),  // Row 1 – Orange
    sf::Color(210, 200,  20),  // Row 2 – Yellow
    sf::Color( 45, 185,  45),  // Row 3 – Green
    sf::Color( 45, 110, 225),  // Row 4 – Blue
    sf::Color(135,  45, 205),  // Row 5 – Purple  (lowest value)
}};

/// Base score awarded per brick in each row (multiplied by hit-point count).
static const std::array<int, Constants::BRICK_ROWS> ROW_POINTS = {{
    60, 50, 40, 30, 20, 10
}};

/// Hit points for each brick row at level 1.  Higher levels add extras.
static const std::array<int, Constants::BRICK_ROWS> ROW_BASE_HIT_POINTS = {{
    1, 1, 1, 1, 1, 1
}};

// =============================================================================
// Construction
// =============================================================================

Game::Game(const std::string& fontPath)
    : window(
        sf::VideoMode(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT),
        Constants::WINDOW_TITLE,
        sf::Style::Titlebar | sf::Style::Close)
    , ball(
        Constants::WINDOW_WIDTH  * 0.5f,
        Constants::WINDOW_HEIGHT * 0.5f,
        Constants::BALL_RADIUS)
    , paddle(
        (Constants::WINDOW_WIDTH  - Constants::PADDLE_WIDTH)  * 0.5f,
        Constants::WINDOW_HEIGHT  - Constants::PADDLE_Y_OFFSET,
        Constants::PADDLE_WIDTH,
        Constants::PADDLE_HEIGHT,
        Constants::PADDLE_SPEED)
    , state(GameState::MainMenu)
    , score(0)
    , lives(Constants::INITIAL_LIVES)
    , level(1)
    , ballSpeed(Constants::BALL_INITIAL_SPEED)
    , levelCompleteTimer(0.0f)
    , bricksRemaining(0)
    , previousState(GameState::MainMenu)
{
    window.setFramerateLimit(Constants::FRAME_RATE);

    // Seed the RNG used for ball launch-angle randomisation.
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    if (!font.loadFromFile(fontPath))
    {
        std::cerr << "[Breakout] ERROR: Could not load font from \"" << fontPath << "\".\n"
                  << "           Run setup.sh (macOS/Linux) or setup.bat (Windows) first,\n"
                  << "           or place any TTF font at the path above.\n";
        window.close();
        return;
    }

    createBricks();
    resetBallOnPaddle();
}

// =============================================================================
// Public entry point
// =============================================================================

void Game::run()
{
    while (window.isOpen())
    {
        // Measure the time elapsed since the last frame.
        float deltaTime = clock.restart().asSeconds();

        // Cap deltaTime so that dragging the window, pausing in a debugger, or
        // coming back from system sleep does not produce a huge physics jump.
        deltaTime = std::min(deltaTime, 0.05f);

        processEvents();

        // Only run physics-related update() when there is meaningful activity.
        switch (state)
        {
        case GameState::Playing:
        case GameState::BallOnPaddle:
            update(deltaTime);
            break;

        case GameState::LevelComplete:
            // Tick the post-level celebration timer.
            levelCompleteTimer -= deltaTime;
            if (levelCompleteTimer <= 0.0f)
                advanceLevel();
            break;

        default:
            break;
        }

        render();
    }
}

// =============================================================================
// Initialisation helpers
// =============================================================================

void Game::createBricks()
{
    bricks.clear();

    // Extra hit points are added to every brick for each level beyond the first,
    // making later levels progressively harder without changing the layout.
    int extraHitPoints = std::max(0, level - 1);

    // Compute the total grid width so we can centre it within the window.
    float totalGridWidth =
        static_cast<float>(Constants::BRICK_COLS) * Constants::BRICK_WIDTH +
        static_cast<float>(Constants::BRICK_COLS - 1) * Constants::BRICK_PADDING;

    float gridStartX = (static_cast<float>(Constants::WINDOW_WIDTH) - totalGridWidth) * 0.5f;

    for (int row = 0; row < Constants::BRICK_ROWS; ++row)
    {
        for (int col = 0; col < Constants::BRICK_COLS; ++col)
        {
            float x = gridStartX +
                      static_cast<float>(col) * (Constants::BRICK_WIDTH + Constants::BRICK_PADDING);

            float y = Constants::BRICK_TOP_OFFSET +
                      static_cast<float>(row) * (Constants::BRICK_HEIGHT + Constants::BRICK_PADDING);

            int hp     = ROW_BASE_HIT_POINTS[row] + extraHitPoints;
            int points = ROW_POINTS[row] * hp; // More HP → more points when destroyed.

            bricks.emplace_back(x, y,
                                Constants::BRICK_WIDTH, Constants::BRICK_HEIGHT,
                                ROW_COLORS[row], hp, points);
        }
    }

    bricksRemaining = static_cast<int>(bricks.size());
}

void Game::resetBallOnPaddle()
{
    // Place the ball exactly on top of the paddle centre.
    float ballX = paddle.getCentreX();
    float ballY = paddle.getTopY() - Constants::BALL_RADIUS - 1.0f;
    ball.reset(ballX, ballY);

    state = GameState::BallOnPaddle;
}

void Game::restartGame()
{
    score     = 0;
    lives     = Constants::INITIAL_LIVES;
    level     = 1;
    ballSpeed = Constants::BALL_INITIAL_SPEED;

    // Re-centre the paddle.
    paddle.setPositionX(
        (static_cast<float>(Constants::WINDOW_WIDTH) - Constants::PADDLE_WIDTH) * 0.5f);

    createBricks();
    resetBallOnPaddle();
}

void Game::advanceLevel()
{
    ++level;

    // Increase ball speed, but never exceed the maximum.
    ballSpeed = std::min(ballSpeed + Constants::BALL_SPEED_STEP, Constants::BALL_MAX_SPEED);

    // Re-centre the paddle for the new level.
    paddle.setPositionX(
        (static_cast<float>(Constants::WINDOW_WIDTH) - Constants::PADDLE_WIDTH) * 0.5f);

    createBricks();
    resetBallOnPaddle();
}

// =============================================================================
// Main loop steps
// =============================================================================

void Game::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
            return;
        }

        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::Escape:
                // From the Controls screen, Esc returns to the previous state
                // rather than quitting so the player doesn't lose their game.
                if (state == GameState::Controls)
                    state = previousState;
                else
                    window.close();
                break;

            case sf::Keyboard::Space:
                if (state == GameState::MainMenu)
                {
                    restartGame();
                }
                else if (state == GameState::BallOnPaddle)
                {
                    ball.launch(ballSpeed);
                    state = GameState::Playing;
                }
                else if (state == GameState::GameOver ||
                         state == GameState::Victory)
                {
                    restartGame();
                }
                break;

            case sf::Keyboard::P:
                if (state == GameState::Playing)
                    state = GameState::Paused;
                else if (state == GameState::Paused)
                    state = GameState::Playing;
                break;

            case sf::Keyboard::H:
                // Open the Controls screen from the main menu or while paused.
                // Store the current state so we can return to the right place.
                if (state == GameState::MainMenu || state == GameState::Paused)
                {
                    previousState = state;
                    state         = GameState::Controls;
                }
                else if (state == GameState::Controls)
                {
                    // H also closes the Controls screen.
                    state = previousState;
                }
                break;

            default:
                break;
            }
        }
    }
}

void Game::update(float deltaTime)
{
    // Always move the paddle regardless of ball state so the player can
    // position it before launching.
    paddle.update(deltaTime, static_cast<float>(Constants::WINDOW_WIDTH));

    // While the ball is on the paddle, keep it anchored to the paddle centre
    // so it tracks along as the player moves.
    if (state == GameState::BallOnPaddle)
    {
        float ballX = paddle.getCentreX();
        float ballY = paddle.getTopY() - Constants::BALL_RADIUS - 1.0f;
        ball.reset(ballX, ballY);
        return;
    }

    // From here on the ball is in motion.
    ball.update(deltaTime);

    handleWallCollisions();
    handlePaddleCollision();
    handleBrickCollisions();

    // Check for level-complete or overall victory.
    if (bricksRemaining <= 0)
    {
        if (level >= Constants::MAX_LEVELS)
        {
            state = GameState::Victory;
        }
        else
        {
            state              = GameState::LevelComplete;
            levelCompleteTimer = Constants::LEVEL_COMPLETE_DELAY;
        }
    }
}

void Game::render()
{
    // Deep navy background.
    window.clear(sf::Color(12, 12, 28));

    // Draw all game objects even behind overlays so the background is visible.
    for (const Brick& brick : bricks)
        brick.draw(window);

    paddle.draw(window);
    ball.draw(window);

    // HUD is always shown except on the main menu and controls screen
    // (neither has an active game to report on).
    if (state != GameState::MainMenu && state != GameState::Controls)
        drawHUD();

    // State-specific overlays and hints.
    switch (state)
    {
    case GameState::MainMenu:
    case GameState::Paused:
    case GameState::LevelComplete:
    case GameState::GameOver:
    case GameState::Victory:
        drawStateOverlay();
        break;

    case GameState::Controls:
        drawControlsScreen();
        break;

    case GameState::BallOnPaddle:
    {
        // Small instruction hint at the very bottom of the screen.
        sf::Text hint = makeText("Press SPACE to launch",
                                 Constants::FONT_SIZE_SMALL,
                                 sf::Color(180, 180, 180));
        centreTextHorizontally(hint,
            static_cast<float>(Constants::WINDOW_HEIGHT) - 26.0f);
        window.draw(hint);
        break;
    }

    case GameState::Playing:
        // Active gameplay: no overlay.
        break;
    }

    window.display();
}

// =============================================================================
// Collision helpers
// =============================================================================

void Game::handleWallCollisions()
{
    sf::Vector2f pos    = ball.getPosition();
    float        radius = ball.getRadius();
    float        winW   = static_cast<float>(Constants::WINDOW_WIDTH);
    float        winH   = static_cast<float>(Constants::WINDOW_HEIGHT);

    // Left wall – reflect rightward.
    if (pos.x - radius < 0.0f)
    {
        ball.setVelocityX(std::abs(ball.getVelocity().x));
        ball.setPosition(radius, pos.y);
    }

    // Right wall – reflect leftward.
    if (pos.x + radius > winW)
    {
        ball.setVelocityX(-std::abs(ball.getVelocity().x));
        ball.setPosition(winW - radius, pos.y);
    }

    // Top wall – reflect downward.
    if (pos.y - radius < 0.0f)
    {
        ball.setVelocityY(std::abs(ball.getVelocity().y));
        ball.setPosition(pos.x, radius);
    }

    // Bottom boundary – player has missed the ball.
    if (pos.y - radius > winH)
    {
        --lives;
        if (lives <= 0)
        {
            lives = 0;
            state = GameState::GameOver;
        }
        else
        {
            resetBallOnPaddle();
        }
    }
}

void Game::handlePaddleCollision()
{
    // Only process collisions while the ball is heading downward; this prevents
    // the ball from being deflected a second time while it is still passing
    // through the paddle shape after the first bounce.
    if (ball.getVelocity().y <= 0.0f)
        return;

    sf::FloatRect paddleBounds = paddle.getBounds();
    sf::Vector2f  ballPos      = ball.getPosition();
    float         radius       = ball.getRadius();

    // Broad-phase AABB check: expand the paddle rectangle by the ball radius
    // in every direction, then test whether the ball centre falls inside.
    sf::FloatRect expandedBounds = {
        paddleBounds.left   - radius,
        paddleBounds.top    - radius,
        paddleBounds.width  + 2.0f * radius,
        paddleBounds.height + 2.0f * radius
    };

    if (!expandedBounds.contains(ballPos))
        return;

    // Nudge the ball just above the paddle surface to prevent it sinking in.
    ball.setPosition(ballPos.x, paddleBounds.top - radius - 0.5f);

    // Map the horizontal hit position to a deflection angle.
    // hitOffset is in [-1, 1]: -1 = far left edge, 0 = centre, +1 = far right.
    float hitOffset = (ballPos.x - paddle.getCentreX()) /
                      (paddle.getWidth() * 0.5f);
    hitOffset = std::max(-1.0f, std::min(1.0f, hitOffset));

    // Angles range from -75° (far left) to +75° (far right) relative to
    // straight upward, giving the player meaningful directional control.
    static constexpr float MAX_ANGLE_RAD = 75.0f * (3.14159265f / 180.0f);
    float angle = hitOffset * MAX_ANGLE_RAD;

    float speed = ball.getSpeed();
    ball.setVelocityX( speed * std::sin(angle));  // Positive = rightward.
    ball.setVelocityY(-speed * std::cos(angle));  // Negative = upward in SFML.

    // Re-normalise to compensate for any floating-point error in sin/cos.
    ball.normaliseSpeed(ballSpeed);
}

void Game::handleBrickCollisions()
{
    sf::Vector2f ballCenter = ball.getPosition();
    float        radius     = ball.getRadius();

    // Reflect the ball at most once per frame to avoid erratic behaviour when
    // the ball grazes the corner shared by two adjacent bricks.
    bool collisionResolvedThisFrame = false;

    for (Brick& brick : bricks)
    {
        if (brick.isDestroyed())
            continue;

        sf::FloatRect rect = brick.getBounds();

        // Nearest-point circle–AABB test:
        // find the closest point on the brick rectangle to the ball centre.
        float closestX = std::max(rect.left, std::min(ballCenter.x, rect.left + rect.width));
        float closestY = std::max(rect.top,  std::min(ballCenter.y, rect.top  + rect.height));

        float dx     = ballCenter.x - closestX;
        float dy     = ballCenter.y - closestY;
        float distSq = dx * dx + dy * dy;

        // No intersection if the nearest point is farther than the radius.
        if (distSq >= radius * radius)
            continue;

        // -----------------------------------------------------------------
        // Collision confirmed – damage the brick.
        // -----------------------------------------------------------------
        brick.hit();

        if (brick.isDestroyed())
        {
            score += brick.getPoints();
            --bricksRemaining;
        }

        // -----------------------------------------------------------------
        // Resolve the ball reflection (first hit only this frame).
        // -----------------------------------------------------------------
        if (!collisionResolvedThisFrame)
        {
            // Compute the collision normal from nearest-point to ball centre.
            float dist = std::sqrt(distSq);

            sf::Vector2f normal;
            if (dist > 0.0001f)
            {
                normal = { dx / dist, dy / dist };
            }
            else
            {
                // The ball centre is exactly inside the rectangle – use
                // a safe default upward normal.
                normal = { 0.0f, -1.0f };
            }

            reflectBall(normal);

            // Push the ball clear of the brick surface along the normal.
            float penetrationDepth = radius - dist;
            ball.setPosition(
                ballCenter.x + normal.x * (penetrationDepth + 0.5f),
                ballCenter.y + normal.y * (penetrationDepth + 0.5f));

            // Normalise speed to counteract accumulated floating-point drift.
            ball.normaliseSpeed(ballSpeed);

            collisionResolvedThisFrame = true;
        }
    }
}

void Game::reflectBall(sf::Vector2f normal)
{
    // Standard specular reflection: r = v − 2(v·n)n
    sf::Vector2f vel = ball.getVelocity();
    float dot        = vel.x * normal.x + vel.y * normal.y;

    ball.setVelocityX(vel.x - 2.0f * dot * normal.x);
    ball.setVelocityY(vel.y - 2.0f * dot * normal.y);
}

// =============================================================================
// Render helpers
// =============================================================================

void Game::drawHUD()
{
    std::ostringstream oss;

    // ---- Score (left-aligned) ----
    oss << "Score: " << score;
    sf::Text scoreText = makeText(oss.str(), Constants::FONT_SIZE_MEDIUM, sf::Color::White);
    scoreText.setPosition(10.0f, 4.0f);
    window.draw(scoreText);

    // ---- Level (centred) ----
    oss.str("");
    oss << "Level: " << level;
    sf::Text levelText = makeText(oss.str(), Constants::FONT_SIZE_MEDIUM, sf::Color::White);
    centreTextHorizontally(levelText, 4.0f);
    window.draw(levelText);

    // ---- Life indicators – small circles at the bottom-right ----
    float indicatorDiameter = Constants::LIFE_INDICATOR_RADIUS * 2.0f;
    float totalIndicatorWidth =
        static_cast<float>(Constants::INITIAL_LIVES) * indicatorDiameter +
        static_cast<float>(Constants::INITIAL_LIVES - 1) * Constants::LIFE_INDICATOR_GAP;

    float indicatorStartX =
        static_cast<float>(Constants::WINDOW_WIDTH) - totalIndicatorWidth - 10.0f;
    float indicatorY =
        static_cast<float>(Constants::WINDOW_HEIGHT) - indicatorDiameter - 6.0f;

    for (int i = 0; i < Constants::INITIAL_LIVES; ++i)
    {
        sf::CircleShape lifeCircle(Constants::LIFE_INDICATOR_RADIUS);
        lifeCircle.setOrigin(Constants::LIFE_INDICATOR_RADIUS,
                             Constants::LIFE_INDICATOR_RADIUS);

        // Fill only the circles representing lives the player still has.
        if (i < lives)
        {
            lifeCircle.setFillColor(sf::Color::White);
            lifeCircle.setOutlineColor(sf::Color(180, 180, 180));
        }
        else
        {
            lifeCircle.setFillColor(sf::Color::Transparent);
            lifeCircle.setOutlineColor(sf::Color(90, 90, 90));
        }
        lifeCircle.setOutlineThickness(1.5f);

        float x = indicatorStartX +
                  static_cast<float>(i) *
                  (indicatorDiameter + Constants::LIFE_INDICATOR_GAP) +
                  Constants::LIFE_INDICATOR_RADIUS;

        lifeCircle.setPosition(x, indicatorY + Constants::LIFE_INDICATOR_RADIUS);
        window.draw(lifeCircle);
    }
}

void Game::drawStateOverlay()
{
    // Semi-transparent dark backdrop so game objects are still faintly visible.
    sf::RectangleShape backdrop(
        sf::Vector2f(static_cast<float>(Constants::WINDOW_WIDTH),
                     static_cast<float>(Constants::WINDOW_HEIGHT)));
    backdrop.setFillColor(sf::Color(0, 0, 0, 170));
    window.draw(backdrop);

    float midY = static_cast<float>(Constants::WINDOW_HEIGHT) * 0.5f;

    switch (state)
    {
    // ---- Main Menu ----
    case GameState::MainMenu:
    {
        sf::Text title = makeText("BREAKOUT", Constants::FONT_SIZE_LARGE, sf::Color::Yellow);
        centreTextHorizontally(title, midY - 90.0f);
        window.draw(title);

        sf::Text startPrompt = makeText("Press SPACE to start",
                                        Constants::FONT_SIZE_MEDIUM,
                                        sf::Color::White);
        centreTextHorizontally(startPrompt, midY - 15.0f);
        window.draw(startPrompt);

        sf::Text controlsHint = makeText("Press H for controls",
                                          Constants::FONT_SIZE_MEDIUM,
                                          sf::Color(100, 220, 255));
        centreTextHorizontally(controlsHint, midY + 25.0f);
        window.draw(controlsHint);

        sf::Text quitHint = makeText("ESC to quit",
                                      Constants::FONT_SIZE_SMALL,
                                      sf::Color(130, 130, 130));
        centreTextHorizontally(quitHint, midY + 68.0f);
        window.draw(quitHint);
        break;
    }

    // ---- Paused ----
    case GameState::Paused:
    {
        sf::Text pauseLabel = makeText("PAUSED", Constants::FONT_SIZE_LARGE, sf::Color::Cyan);
        centreTextHorizontally(pauseLabel, midY - 50.0f);
        window.draw(pauseLabel);

        sf::Text resumeHint = makeText("P — Resume",
                                       Constants::FONT_SIZE_MEDIUM,
                                       sf::Color::White);
        centreTextHorizontally(resumeHint, midY + 10.0f);
        window.draw(resumeHint);

        sf::Text controlsHint = makeText("H — Controls",
                                          Constants::FONT_SIZE_MEDIUM,
                                          sf::Color(100, 220, 255));
        centreTextHorizontally(controlsHint, midY + 42.0f);
        window.draw(controlsHint);
        break;
    }

    // ---- Level Complete ----
    case GameState::LevelComplete:
    {
        std::string message = "Level " + std::to_string(level) + " Complete!";
        sf::Text levelDone = makeText(message, Constants::FONT_SIZE_LARGE, sf::Color::Green);
        centreTextHorizontally(levelDone, midY - 30.0f);
        window.draw(levelDone);

        sf::Text nextLevel = makeText("Get ready for level " + std::to_string(level + 1) + "...",
                                      Constants::FONT_SIZE_MEDIUM,
                                      sf::Color(180, 255, 180));
        centreTextHorizontally(nextLevel, midY + 25.0f);
        window.draw(nextLevel);
        break;
    }

    // ---- Game Over ----
    case GameState::GameOver:
    {
        sf::Text gameOverLabel = makeText("GAME OVER",
                                          Constants::FONT_SIZE_LARGE,
                                          sf::Color(255, 60, 60));
        centreTextHorizontally(gameOverLabel, midY - 65.0f);
        window.draw(gameOverLabel);

        std::ostringstream oss;
        oss << "Final Score: " << score;
        sf::Text finalScore = makeText(oss.str(),
                                       Constants::FONT_SIZE_MEDIUM,
                                       sf::Color::White);
        centreTextHorizontally(finalScore, midY - 5.0f);
        window.draw(finalScore);

        sf::Text restartHint = makeText("Press SPACE to restart",
                                        Constants::FONT_SIZE_MEDIUM,
                                        sf::Color(200, 200, 200));
        centreTextHorizontally(restartHint, midY + 40.0f);
        window.draw(restartHint);
        break;
    }

    // ---- Victory ----
    case GameState::Victory:
    {
        sf::Text victoryLabel = makeText("YOU WIN!",
                                         Constants::FONT_SIZE_LARGE,
                                         sf::Color::Yellow);
        centreTextHorizontally(victoryLabel, midY - 65.0f);
        window.draw(victoryLabel);

        std::ostringstream oss;
        oss << "Final Score: " << score;
        sf::Text finalScore = makeText(oss.str(),
                                       Constants::FONT_SIZE_MEDIUM,
                                       sf::Color::White);
        centreTextHorizontally(finalScore, midY - 5.0f);
        window.draw(finalScore);

        sf::Text playAgainHint = makeText("Press SPACE to play again",
                                          Constants::FONT_SIZE_MEDIUM,
                                          sf::Color(200, 200, 200));
        centreTextHorizontally(playAgainHint, midY + 40.0f);
        window.draw(playAgainHint);
        break;
    }

    default:
        break;
    }
}

void Game::centreTextHorizontally(sf::Text& text, float y)
{
    sf::FloatRect bounds = text.getGlobalBounds();
    float x = (static_cast<float>(Constants::WINDOW_WIDTH) - bounds.width) * 0.5f;
    text.setPosition(std::max(0.0f, x), y);
}

sf::Text Game::makeText(const std::string& content,
                         unsigned int characterSize,
                         sf::Color color) const
{
    sf::Text text;
    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(characterSize);
    text.setFillColor(color);
    return text;
}

void Game::drawControlsScreen()
{
    // -------------------------------------------------------------------------
    // Full-screen dark backdrop.
    // -------------------------------------------------------------------------
    sf::RectangleShape backdrop(
        sf::Vector2f(static_cast<float>(Constants::WINDOW_WIDTH),
                     static_cast<float>(Constants::WINDOW_HEIGHT)));
    backdrop.setFillColor(sf::Color(0, 0, 0, 210));
    window.draw(backdrop);

    // Fixed column X positions for the two-column key / description layout.
    const float keyColumnX  = 170.0f;   ///< Right-edge of the key-label column.
    const float descColumnX = 210.0f;   ///< Left-edge of the description column.

    // -------------------------------------------------------------------------
    // Helper lambda: draw a key label (right-aligned to keyColumnX) and its
    // description (left-aligned at descColumnX) on the same row.
    // -------------------------------------------------------------------------
    auto drawRow = [&](const std::string& keyLabel,
                       const std::string& description,
                       float y,
                       sf::Color keyColor   = sf::Color(255, 220, 80),
                       sf::Color descColor  = sf::Color(220, 220, 220))
    {
        sf::Text keyText = makeText(keyLabel, Constants::FONT_SIZE_SMALL, keyColor);
        // Right-align the key label so all keys end at the same X.
        float keyWidth = keyText.getGlobalBounds().width;
        keyText.setPosition(keyColumnX - keyWidth, y);
        window.draw(keyText);

        sf::Text descText = makeText(description, Constants::FONT_SIZE_SMALL, descColor);
        descText.setPosition(descColumnX, y);
        window.draw(descText);
    };

    // -------------------------------------------------------------------------
    // Helper lambda: draw a thin horizontal separator rule.
    // -------------------------------------------------------------------------
    auto drawRule = [&](float y)
    {
        sf::RectangleShape rule(sf::Vector2f(
            static_cast<float>(Constants::WINDOW_WIDTH) - 120.0f, 1.0f));
        rule.setFillColor(sf::Color(80, 80, 80));
        rule.setPosition(60.0f, y);
        window.draw(rule);
    };

    // -------------------------------------------------------------------------
    // Helper lambda: draw a section header, left-aligned at keyColumnX.
    // -------------------------------------------------------------------------
    auto drawSectionHeader = [&](const std::string& title, float y)
    {
        sf::Text header = makeText(title, Constants::FONT_SIZE_SMALL,
                                   sf::Color(140, 200, 255));
        header.setPosition(keyColumnX - header.getGlobalBounds().width, y);
        window.draw(header);
    };

    // =========================================================================
    // Title
    // =========================================================================
    sf::Text titleText = makeText("CONTROLS", Constants::FONT_SIZE_LARGE, sf::Color::White);
    centreTextHorizontally(titleText, 18.0f);
    window.draw(titleText);

    drawRule(72.0f);

    // =========================================================================
    // Section: Movement
    // =========================================================================
    float y = 84.0f;
    drawSectionHeader("MOVEMENT", y);
    y += 26.0f;
    drawRow("\u2190 / A", "Move paddle left",  y);
    y += 24.0f;
    drawRow("\u2192 / D", "Move paddle right", y);

    y += 34.0f;
    drawRule(y);

    // =========================================================================
    // Section: Game controls
    // =========================================================================
    y += 12.0f;
    drawSectionHeader("GAME", y);
    y += 26.0f;
    drawRow("Space", "Launch ball  /  Start  /  Restart", y);
    y += 24.0f;
    drawRow("P",     "Pause / Resume",                    y);
    y += 24.0f;
    drawRow("H",     "Show / hide this screen",           y);
    y += 24.0f;
    drawRow("Esc",   "Close controls  /  Quit",           y);

    y += 34.0f;
    drawRule(y);

    // =========================================================================
    // Section: Scoring
    // =========================================================================
    y += 12.0f;
    drawSectionHeader("SCORING", y);
    y += 26.0f;

    // One entry per brick row; colours match ROW_COLORS in the game.
    struct ScoringEntry { sf::Color color; std::string label; int points; };
    static const ScoringEntry scoringTable[] = {
        { sf::Color(220,  45,  45), "Red    row", 60 },
        { sf::Color(230, 120,  20), "Orange row", 50 },
        { sf::Color(210, 200,  20), "Yellow row", 40 },
        { sf::Color( 45, 185,  45), "Green  row", 30 },
        { sf::Color( 45, 110, 225), "Blue   row", 20 },
        { sf::Color(135,  45, 205), "Purple row", 10 },
    };

    const float dotRadius  = 5.0f;
    const float rowSpacing = 22.0f;

    for (const ScoringEntry& entry : scoringTable)
    {
        // Coloured dot matching the brick colour.
        sf::CircleShape dot(dotRadius);
        dot.setOrigin(dotRadius, dotRadius);
        dot.setFillColor(entry.color);
        dot.setPosition(keyColumnX - dotRadius * 2.0f - 2.0f,
                        y + dotRadius + 1.0f);
        window.draw(dot);

        // Points value in key column colour.
        std::string pointsStr = std::to_string(entry.points) + " pts";
        sf::Text pointsText = makeText(pointsStr, Constants::FONT_SIZE_SMALL,
                                       sf::Color(255, 220, 80));
        float pw = pointsText.getGlobalBounds().width;
        pointsText.setPosition(keyColumnX - dotRadius * 2.0f - 2.0f - pw - 6.0f, y);
        window.draw(pointsText);

        // Row label.
        sf::Text labelText = makeText(entry.label, Constants::FONT_SIZE_SMALL,
                                      sf::Color(220, 220, 220));
        labelText.setPosition(descColumnX, y);
        window.draw(labelText);

        y += rowSpacing;
    }

    // Footnote explaining multi-hit bricks on higher levels.
    y += 4.0f;
    sf::Text footnote = makeText(
        "Higher levels add hit points per brick; score = base x hit points.",
        Constants::FONT_SIZE_SMALL - 2,
        sf::Color(120, 120, 120));
    centreTextHorizontally(footnote, y);
    window.draw(footnote);

    // =========================================================================
    // Return hint at the bottom.
    // =========================================================================
    sf::Text returnHint = makeText("H or Esc  \u2014  Return",
                                    Constants::FONT_SIZE_SMALL,
                                    sf::Color(100, 220, 255));
    centreTextHorizontally(returnHint,
        static_cast<float>(Constants::WINDOW_HEIGHT) - 30.0f);
    window.draw(returnHint);
}
