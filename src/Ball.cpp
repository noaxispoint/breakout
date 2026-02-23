/**
 * @file Ball.cpp
 * @brief Implementation of the Ball class.
 */

#include "Ball.hpp"

#include <cmath>
#include <cstdlib>

/// Mathematical constant π used for degree-to-radian conversions.
static constexpr float PI = 3.14159265358979323846f;

/**
 * @brief Converts an angle in degrees to radians.
 * @param degrees  Angle in degrees.
 * @return float   Equivalent angle in radians.
 */
static float toRadians(float degrees)
{
    return degrees * (PI / 180.0f);
}

// -----------------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------------

Ball::Ball(float startX, float startY, float radius)
    : velocity(0.0f, 0.0f) // Initialiser order must match member declaration order in Ball.hpp.
    , radius(radius)
    , moving(false)
{
    // Centre the shape's local origin so that setPosition/getPosition refer
    // to the geometric centre of the circle rather than its top-left corner.
    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    shape.setPosition(startX, startY);

    // Visual appearance: white fill with a subtle grey outline.
    shape.setFillColor(sf::Color::White);
    shape.setOutlineThickness(1.5f);
    shape.setOutlineColor(sf::Color(180, 180, 180));
}

// -----------------------------------------------------------------------------
// Per-frame update and rendering
// -----------------------------------------------------------------------------

void Ball::update(float deltaTime)
{
    if (!moving)
        return;

    // Advance position by velocity * time; standard Euler integration.
    shape.move(velocity * deltaTime);
}

void Ball::draw(sf::RenderWindow& window) const
{
    window.draw(shape);
}

// -----------------------------------------------------------------------------
// Movement control
// -----------------------------------------------------------------------------

void Ball::launch(float speed)
{
    // Ignore repeated launch calls while the ball is already in flight.
    if (moving)
        return;

    // Choose a random launch angle offset in [-45°, +45°] from straight up.
    // std::rand is seeded once in main() via std::srand.
    float angleOffsetDeg = static_cast<float>(std::rand() % 91) - 45.0f;

    // Straight upward in SFML is -90° (y-axis points down).
    float angleRad = toRadians(-90.0f + angleOffsetDeg);

    velocity.x = speed * std::cos(angleRad);
    velocity.y = speed * std::sin(angleRad);
    moving = true;
}

void Ball::reset(float x, float y)
{
    shape.setPosition(x, y);
    velocity = {0.0f, 0.0f};
    moving   = false;
}

void Ball::setPosition(float x, float y)
{
    shape.setPosition(x, y);
}

// -----------------------------------------------------------------------------
// Velocity manipulation
// -----------------------------------------------------------------------------

void Ball::reverseVelocityX()
{
    velocity.x = -velocity.x;
}

void Ball::reverseVelocityY()
{
    velocity.y = -velocity.y;
}

void Ball::setVelocityX(float vx)
{
    velocity.x = vx;
}

void Ball::setVelocityY(float vy)
{
    velocity.y = vy;
}

void Ball::normaliseSpeed(float speed)
{
    float currentSpeed = getSpeed();

    // Guard against division by zero (ball is effectively stationary).
    if (currentSpeed < 0.0001f)
        return;

    // Scale both components so the magnitude equals the target speed.
    velocity = (velocity / currentSpeed) * speed;
}

// -----------------------------------------------------------------------------
// Accessors
// -----------------------------------------------------------------------------

sf::FloatRect Ball::getBounds() const
{
    return shape.getGlobalBounds();
}

sf::Vector2f Ball::getPosition() const
{
    return shape.getPosition();
}

sf::Vector2f Ball::getVelocity() const
{
    return velocity;
}

float Ball::getRadius() const
{
    return radius;
}

bool Ball::isMoving() const
{
    return moving;
}

float Ball::getSpeed() const
{
    return std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
}
