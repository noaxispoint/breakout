/**
 * @file Ball.hpp
 * @brief Declaration of the Ball class.
 *
 * The Ball is the central gameplay object: it bounces continuously around the
 * playfield, deflecting off walls, the paddle, and bricks.  Speed can be
 * increased between levels to raise the difficulty.
 */

#pragma once

#include <SFML/Graphics.hpp>

/**
 * @brief The bouncing ball in the Breakout playfield.
 *
 * Ball renders itself as an sf::CircleShape and maintains its own velocity
 * vector.  All position arithmetic uses a centred origin, so getPosition()
 * always returns the geometric centre of the circle.
 *
 * The ball starts stationary; call launch() to begin movement.  Call reset()
 * to return it to a position and stop it (e.g. after a life is lost).
 */
class Ball
{
public:
    /**
     * @brief Constructs a Ball at the given position.
     *
     * The ball begins stationary (isMoving() returns false).  Call launch() to
     * start it moving.
     *
     * @param startX  Initial X coordinate of the ball's centre, in pixels.
     * @param startY  Initial Y coordinate of the ball's centre, in pixels.
     * @param radius  Radius of the ball circle, in pixels.
     */
    Ball(float startX, float startY, float radius);

    /**
     * @brief Advances the ball's position by one simulation step.
     *
     * Moves the ball by velocity * deltaTime.  Has no effect when the ball is
     * not moving (i.e. before launch() has been called or after reset()).
     *
     * @param deltaTime  Elapsed time since the previous frame, in seconds.
     */
    void update(float deltaTime);

    /**
     * @brief Draws the ball onto the given render window.
     * @param window  The sf::RenderWindow to draw into.
     */
    void draw(sf::RenderWindow& window) const;

    /**
     * @brief Launches the ball upward at a randomised angle.
     *
     * The launch direction is between -45° and +45° from straight upward so
     * the ball always moves toward the bricks.  The speed magnitude equals
     * the @p speed parameter.  Has no effect if the ball is already moving.
     *
     * @param speed  Desired initial speed in pixels per second.
     */
    void launch(float speed);

    /**
     * @brief Teleports the ball to (@p x, @p y) and stops all movement.
     *
     * Typically called after losing a life to place the ball back on top of
     * the paddle.
     *
     * @param x  New centre X coordinate, in pixels.
     * @param y  New centre Y coordinate, in pixels.
     */
    void reset(float x, float y);

    /**
     * @brief Moves the ball centre to (@p x, @p y) without stopping it.
     *
     * Used for collision-resolution nudges that need to push the ball out of
     * a surface without interrupting gameplay.
     *
     * @param x  New centre X coordinate, in pixels.
     * @param y  New centre Y coordinate, in pixels.
     */
    void setPosition(float x, float y);

    /**
     * @brief Negates the horizontal (X) component of velocity.
     *
     * Called when the ball strikes a vertical surface: the left or right wall,
     * or the left/right face of a brick.
     */
    void reverseVelocityX();

    /**
     * @brief Negates the vertical (Y) component of velocity.
     *
     * Called when the ball strikes a horizontal surface: the top wall, the
     * top/bottom face of a brick, or the paddle.
     */
    void reverseVelocityY();

    /**
     * @brief Directly sets the horizontal velocity component.
     *
     * Used by paddle-deflection logic to steer the ball based on the hit
     * position relative to the paddle centre.
     *
     * @param vx  New horizontal velocity in pixels per second.
     */
    void setVelocityX(float vx);

    /**
     * @brief Directly sets the vertical velocity component.
     * @param vy  New vertical velocity in pixels per second.
     */
    void setVelocityY(float vy);

    /**
     * @brief Rescales the ball's velocity so its magnitude equals @p speed.
     *
     * Repeatedly reflecting a floating-point vector introduces rounding error
     * that slowly drifts the ball's speed.  Calling this method after each
     * bounce keeps the speed consistent.
     *
     * @param speed  Desired speed magnitude in pixels per second.
     */
    void normaliseSpeed(float speed);

    /**
     * @brief Returns the ball's axis-aligned bounding rectangle.
     *
     * The bounding box is a square of side length diameter (2 * radius),
     * suitable for broad-phase intersection tests against rectangular objects.
     *
     * @return sf::FloatRect  Bounding rectangle in world coordinates.
     */
    sf::FloatRect getBounds() const;

    /**
     * @brief Returns the ball's centre position in world coordinates.
     * @return sf::Vector2f  Centre (x, y) of the ball.
     */
    sf::Vector2f getPosition() const;

    /**
     * @brief Returns the ball's current velocity vector.
     * @return sf::Vector2f  Velocity components (pixels per second).
     */
    sf::Vector2f getVelocity() const;

    /**
     * @brief Returns the ball's radius.
     * @return float  Radius in pixels.
     */
    float getRadius() const;

    /**
     * @brief Reports whether the ball is currently in motion.
     * @return true after launch() has been called; false after reset().
     */
    bool isMoving() const;

    /**
     * @brief Computes the current scalar speed of the ball.
     *
     * Equivalent to the Euclidean magnitude of the velocity vector.
     *
     * @return float  Speed in pixels per second.
     */
    float getSpeed() const;

private:
    sf::CircleShape shape;    ///< SFML renderable circle (origin centred).
    sf::Vector2f    velocity; ///< Current velocity vector, pixels per second.
    float           radius;   ///< Cached ball radius in pixels.
    bool            moving;   ///< True once launch() has been called.
};
