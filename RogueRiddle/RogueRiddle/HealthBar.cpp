#include <SFML/Graphics.hpp>
#include "HealthBar.h"
#include <iostream>

HealthBar::HealthBar() {
    m_background.setSize(sf::Vector2f(m_maxWidth, m_height));
    m_background.setFillColor(sf::Color::Black);
    m_bar.setSize(sf::Vector2f(m_currentWidth, m_height));
    m_bar.setFillColor(sf::Color(0, 128, 0));
}

void HealthBar::setHealth(float health) {
    if (health < 0) {
        health = 0;
    }

    m_currentWidth = (health / 100.0f) * m_maxWidth;

    m_bar.setSize(sf::Vector2f(m_currentWidth, m_height));
    m_bar.setFillColor(sf::Color(0, 128, 0));
}

void HealthBar::draw(sf::RenderWindow& window, float x, float y) {
    m_background.setPosition(x, y);
    m_bar.setPosition(x, y);

    window.draw(m_background);
    window.draw(m_bar);
}
