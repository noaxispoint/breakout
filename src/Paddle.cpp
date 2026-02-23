/**
 * @file Paddle.cpp
 * @brief Implementation of the Paddle class.
 */

#include "Paddle.hpp"

#include <algorithm> // std::max, std::min

// -----------------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------------

Paddle::Paddle(float startX, float startY, float width, float height, float speed)
    : speed(speed)
    , width(width)
    , height(height)
{
    shape.setSize({width, height});
    shape.setPosition(startX, startY);

    // Light blue fill with a darker outline to stand out on the dark background.
    shape.setFillColor(sf::Color(100, 180, 255));
    shape.setOutlineThickness(1.5f);
    shape.setOutlineColor(sf::Color(50, 130, 210));
}

// -----------------------------------------------------------------------------
// Per-frame update and rendering
// -----------------------------------------------------------------------------

void Paddle::update(float deltaTime, float windowWidth)
{
    float horizontalInput = 0.0f;

    // Accept both arrow keys and WASD so players can use either scheme.
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        horizontalInput -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        horizontalInput += 1.0f;
    }

    // Compute the new left-edge X, clamped so the paddle stays within the window.
    float newX = shape.getPosition().x + horizontalInput * speed * deltaTime;
    newX = std::max(0.0f, std::min(newX, windowWidth - width));

    shape.setPosition(newX, shape.getPosition().y);
}

void Paddle::draw(sf::RenderWindow& window) const
{
    window.draw(shape);
}

// -----------------------------------------------------------------------------
// Mutators
// -----------------------------------------------------------------------------

void Paddle::setPositionX(float x)
{
    shape.setPosition(x, shape.getPosition().y);
}

// -----------------------------------------------------------------------------
// Accessors
// -----------------------------------------------------------------------------

sf::FloatRect Paddle::getBounds() const
{
    return shape.getGlobalBounds();
}

float Paddle::getCentreX() const
{
    return shape.getPosition().x + width * 0.5f;
}

float Paddle::getTopY() const
{
    return shape.getPosition().y;
}

float Paddle::getWidth() const
{
    return width;
}

float Paddle::getHeight() const
{
    return height;
}
