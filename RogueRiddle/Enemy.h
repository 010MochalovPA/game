#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "AnimationManager.h"
#include "HealthBar.h"
#include "IEntity.h"

class Level;

struct DamageText {
	sf::Text text;
	float duration;
	sf::Uint8 initialAlpha;
};

class Enemy : IEntity, std::enable_shared_from_this<Enemy> {
public:
	Enemy(Level& level, float x, float y, float patrolDistanse);
	void update(float time);
	void checkCollision(float velocityX);
	void setAnimation();
	void draw(sf::RenderWindow& window);
	void takeDamage(int damage, bool damageFromRight);
	float getX();
	float getY();
	float getWidth();
	float getHeight();
	void meleeAttack();
	bool collisionX(float velocityX);
	bool isDead();
	void updateDamageTexts(float time);
	DamageText createDamageText(int damage);

private:
	sf::Vector2f m_velocity = { 0, 0 };
	sf::Vector2f m_position = { 120.f, 40.f };
	float m_width = 20;
	float m_height = 32;
	AnimationManager m_animationManager;
	State m_state;
	Direction m_direction = Direction::RIGHT;
	sf::Texture m_texture;
	float m_maxSpeed;
	float m_acceleration;
	Level* m_level;
	HealthBar m_healthBar;
	float m_patrolDistance;
	float m_patrolStartX;
	float m_patrolEndX;
	bool m_isPatrolling;
	int m_health;
	float m_attackTimer = 0.0f;
	bool m_isOnCooldown;
	float m_cooldownTime;
	float m_currentCooldownTime;
	std::vector<DamageText> m_damageTexts;
	sf::Font m_font;
	bool m_isDead;
	sf::Clock m_attackClock;
	sf::Time m_attackCooldown;
};