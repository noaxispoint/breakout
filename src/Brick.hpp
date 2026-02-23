/**
 * @file Brick.hpp
 * @brief Declaration of the Brick class.
 *
 * A Brick is a rectangular target that the ball must strike to destroy.
 * Bricks have a configurable number of hit points; multi-hit bricks appear
 * on higher difficulty levels.  The visual colour darkens proportionally as
 * the brick takes damage, giving the player clear feedback.
 */

#pragma once

#include <SFML/Graphics.hpp>

/**
 * @brief A single destructible brick in the Breakout playfield.
 *
 * Each brick tracks its remaining hit points.  When hit points reach zero the
 * brick is marked as destroyed and excluded from all subsequent rendering and
 * collision checks by the Game class.
 *
 * Colour feedback: the brick fill is interpolated from its full base colour
 * down to 40% brightness as damage is accumulated, so a 3-HP brick visually
 * progresses through three distinct shades.
 */
class Brick
{
public:
    /**
     * @brief Constructs a Brick.
     *
     * @param x         Left edge of the brick, in pixels.
     * @param y         Top edge of the brick, in pixels.
     * @param width     Width of the brick, in pixels.
     * @param height    Height of the brick, in pixels.
     * @param color     Base fill colour at full health.
     * @param hitPoints Number of hits required to destroy this brick (≥ 1).
     * @param points    Score awarded to the player when the brick is destroyed.
     */
    Brick(float x, float y, float width, float height,
          sf::Color color, int hitPoints, int points);

    /**
     * @brief Draws the brick onto the given render window.
     *
     * Does nothing if the brick has already been destroyed.
     *
     * @param window  The sf::RenderWindow to draw into.
     */
    void draw(sf::RenderWindow& window) const;

    /**
     * @brief Registers one hit on this brick.
     *
     * Decrements hit points by one.  If hit points drop to zero the brick is
     * flagged as destroyed and its colour is not updated further.  Otherwise
     * the fill colour darkens to reflect the new damage level.
     *
     * Has no effect if the brick is already destroyed.
     */
    void hit();

    /**
     * @brief Returns whether this brick has been fully destroyed.
     * @return true once hit points have reached zero; false while alive.
     */
    bool isDestroyed() const;

    /**
     * @brief Returns the brick's axis-aligned bounding rectangle.
     *
     * Only meaningful for collision purposes while isDestroyed() is false.
     *
     * @return sf::FloatRect  Bounding rectangle in world coordinates.
     */
    sf::FloatRect getBounds() const;

    /**
     * @brief Returns the score value awarded when this brick is destroyed.
     * @return int  Point value (constant; unaffected by hit()).
     */
    int getPoints() const;

    /**
     * @brief Returns the brick's remaining hit-point count.
     * @return int  Remaining hit points (0 if destroyed).
     */
    int getHitPoints() const;

private:
    /**
     * @brief Recomputes the fill colour to reflect the current damage level.
     *
     * Interpolates each RGB channel from 40% brightness (heavily damaged) up
     * to 100% (full health), based on the ratio of remaining to maximum HP.
     */
    void updateColor();

    sf::RectangleShape shape;       ///< Renderable rectangle.
    sf::Color          baseColor;   ///< Full-health fill colour.
    int                hitPoints;   ///< Current remaining hit points.
    int                maxHitPoints;///< Starting hit points (for colour lerp).
    int                points;      ///< Score awarded on destruction.
    bool               destroyed;   ///< True once hit points reach zero.
};
