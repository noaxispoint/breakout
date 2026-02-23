/**
 * @file Paddle.hpp
 * @brief Declaration of the Paddle class.
 *
 * The Paddle is the player-controlled horizontal bar at the bottom of the
 * screen.  It reads keyboard state each frame and moves accordingly, clamped
 * within the window bounds.
 */

#pragma once

#include <SFML/Graphics.hpp>

/**
 * @brief Player-controlled paddle that deflects the ball.
 *
 * Movement is driven by sf::Keyboard::isKeyPressed queries inside update(),
 * which supports both the arrow keys and the A/D WASD keys simultaneously.
 *
 * The paddle is rendered as an sf::RectangleShape with a light-blue fill and
 * a slightly darker outline so it reads clearly against the dark background.
 */
class Paddle
{
public:
    /**
     * @brief Constructs a Paddle.
     *
     * @param startX  X coordinate of the paddle's left edge, in pixels.
     * @param startY  Y coordinate of the paddle's top edge, in pixels.
     * @param width   Width of the paddle, in pixels.
     * @param height  Height of the paddle, in pixels.
     * @param speed   Horizontal movement speed, in pixels per second.
     */
    Paddle(float startX, float startY, float width, float height, float speed);

    /**
     * @brief Reads player input and moves the paddle one simulation step.
     *
     * Left arrow or A moves left; right arrow or D moves right.  The paddle
     * is clamped so its edges never exceed the window boundaries [0, windowWidth].
     *
     * @param deltaTime    Time elapsed since the previous frame, in seconds.
     * @param windowWidth  Width of the window used as the right clamp boundary.
     */
    void update(float deltaTime, float windowWidth);

    /**
     * @brief Draws the paddle onto the given render window.
     * @param window  The sf::RenderWindow to draw into.
     */
    void draw(sf::RenderWindow& window) const;

    /**
     * @brief Repositions the paddle horizontally.
     *
     * Used when resetting the game or starting a new level to re-centre the
     * paddle without changing its vertical position.
     *
     * @param x  New X coordinate of the paddle's left edge, in pixels.
     */
    void setPositionX(float x);

    /**
     * @brief Returns the paddle's axis-aligned bounding rectangle.
     * @return sf::FloatRect  Bounds in world coordinates.
     */
    sf::FloatRect getBounds() const;

    /**
     * @brief Returns the X coordinate of the paddle's horizontal centre.
     * @return float  Centre X, in pixels.
     */
    float getCentreX() const;

    /**
     * @brief Returns the Y coordinate of the paddle's top edge.
     * @return float  Top Y, in pixels.
     */
    float getTopY() const;

    /**
     * @brief Returns the paddle's width.
     * @return float  Width in pixels.
     */
    float getWidth() const;

    /**
     * @brief Returns the paddle's height.
     * @return float  Height in pixels.
     */
    float getHeight() const;

private:
    sf::RectangleShape shape;  ///< SFML renderable rectangle.
    float              speed;  ///< Movement speed in pixels per second.
    float              width;  ///< Cached width for geometric queries.
    float              height; ///< Cached height for geometric queries.
};
