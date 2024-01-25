#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "AnimationManager.h"
#include "HealthBar.h"
#include "IEntity.h"

class Level;

class Player : IEntity {
public:
    Player(Level& level, float x, float y);
    void updateView(float time);
    void moveLeft(float time);
    void moveRight(float time);
    bool collisionX(float velocityX);
    bool collisionY(float velocityY);
    bool onGround();
    void jump();
    void stay(float time);
    void update(float time);
    sf::View getView() const;
    void draw(sf::RenderWindow& window);
    void meleeAttack();
    float getX();
    float getY();
    float getWidth();
    float getHeight();
    void takeDamage(int damage, bool damageFromRight);
    void drawScore(sf::RenderWindow& window, int score) const;

private:
    sf::Vector2f m_velocity = { 0, 0 };
    sf::Vector2f m_position;
    float m_width = 20;
    float m_height = 32;
    AnimationManager m_animationManager;
    State m_state;
    Direction m_direction = Direction::RIGHT;
    bool m_onGround;
    sf::Vector2f m_viewPoint;
    sf::Texture m_texture;
    float m_maxSpeed;
    float m_acceleration;
    sf::View m_view;
    Level* m_level;
    HealthBar m_healthBar;
    sf::Clock attackClock;
    const sf::Time attackCooldown = sf::milliseconds(500);
    int m_health;
};
