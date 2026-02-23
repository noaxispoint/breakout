/**
 * @file Brick.cpp
 * @brief Implementation of the Brick class.
 */

#include "Brick.hpp"

#include <cstdint> // uint8_t

// -----------------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------------

Brick::Brick(float x, float y, float width, float height,
             sf::Color color, int hitPoints, int points)
    : baseColor(color)
    , hitPoints(hitPoints)
    , maxHitPoints(hitPoints)
    , points(points)
    , destroyed(false)
{
    shape.setSize({width, height});
    shape.setPosition(x, y);

    // Thin dark outline to separate adjacent bricks visually.
    shape.setOutlineThickness(1.5f);
    shape.setOutlineColor(sf::Color(20, 20, 20, 200));

    // Apply the initial (full-health) colour.
    updateColor();
}

// -----------------------------------------------------------------------------
// Per-frame rendering
// -----------------------------------------------------------------------------

void Brick::draw(sf::RenderWindow& window) const
{
    if (destroyed)
        return;

    window.draw(shape);
}

// -----------------------------------------------------------------------------
// Game logic
// -----------------------------------------------------------------------------

void Brick::hit()
{
    if (destroyed)
        return;

    --hitPoints;

    if (hitPoints <= 0)
    {
        hitPoints = 0;
        destroyed = true;
    }
    else
    {
        // Update the visual colour to indicate the new damage level.
        updateColor();
    }
}

// -----------------------------------------------------------------------------
// Accessors
// -----------------------------------------------------------------------------

bool Brick::isDestroyed() const
{
    return destroyed;
}

sf::FloatRect Brick::getBounds() const
{
    return shape.getGlobalBounds();
}

int Brick::getPoints() const
{
    return points;
}

int Brick::getHitPoints() const
{
    return hitPoints;
}

// -----------------------------------------------------------------------------
// Private helpers
// -----------------------------------------------------------------------------

void Brick::updateColor()
{
    // Compute a health fraction in [0, 1]; 1 = full health, 0 = nearly dead.
    float healthFraction = static_cast<float>(hitPoints) /
                           static_cast<float>(maxHitPoints);

    // Scale each channel from 40% (dim) at low health up to 100% at full health.
    // This gives a clear visual progression without making any state look black.
    float brightnessScale = 0.4f + 0.6f * healthFraction;

    uint8_t r = static_cast<uint8_t>(baseColor.r * brightnessScale);
    uint8_t g = static_cast<uint8_t>(baseColor.g * brightnessScale);
    uint8_t b = static_cast<uint8_t>(baseColor.b * brightnessScale);

    shape.setFillColor(sf::Color(r, g, b));
}
