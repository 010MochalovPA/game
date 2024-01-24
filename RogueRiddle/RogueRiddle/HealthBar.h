#pragma once
#include <SFML/Graphics.hpp>

class HealthBar {
public:
    HealthBar();
    void setHealth(float health);
    void draw(sf::RenderWindow& window, float x, float y);

private:
    float m_maxWidth = 20.f;
    float m_height = 3.f;
    float m_currentWidth = m_maxWidth;

    sf::RectangleShape m_background;
    sf::RectangleShape m_bar;
};