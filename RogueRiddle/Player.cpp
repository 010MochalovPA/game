#include <iostream>
#include "Level.h"
#include "Player.h"
#include "Enemy.h"

namespace DefaultPlayerAttributes
{
	constexpr float MaxSpeed = 160.f;
	constexpr float Acceleration = 7.f;
}

Player::Player(Level& level, float x, float y): m_level(&level), m_healthBar()
{
	m_position = { x, y };
	m_texture.loadFromFile("./assets/sprite-sheet.png");
	m_animationManager = AnimationManager();
	m_animationManager.create("stay", m_texture, 6, 0, 20, 32, 6, 10.f, 32);
	m_animationManager.create("run", m_texture, 6, 32, 20, 32, 8, 14.f, 32);
	m_animationManager.create("jump", m_texture, 6, 96, 32, 32, 4, 8, 32);
	m_animationManager.create("attack", m_texture, 6, 128, 32, 32, 1, 1, 32);
	m_state = State::STAY;
	m_maxSpeed = DefaultPlayerAttributes::MaxSpeed;
	m_acceleration = DefaultPlayerAttributes::Acceleration;
	m_view.setSize(1024, 768);
	m_view.zoom(0.25f);
	m_onGround = true;
	m_health = 100;
}

void Player::updateView(float time) {
	float targetViewX = m_position.x + (m_direction == Direction::LEFT ? -60.f : 60.f);
	float accelerationViewX = (targetViewX - m_viewPoint.x) / 0.15f;
	float targetViewY = m_position.y;
	float accelerationViewY = (targetViewY - m_viewPoint.y) / 0.1f;
	int levelWidth = m_level->GetWidth();
	int levelHeight = m_level->GetHeight();
	if (m_viewPoint.x + accelerationViewX * time - 128 < 0) {
		m_viewPoint.x = 129;
	}
	else if (m_viewPoint.x + accelerationViewX * time + 128 > levelWidth * 16) {
		m_viewPoint.x = levelWidth * 16 - 129;
	}
	else {
		m_viewPoint.x += accelerationViewX * time;
	}

	if (m_viewPoint.y + accelerationViewY * time - 96 < 0) {
		m_viewPoint.y = 97;
	}
	else if (m_viewPoint.y + accelerationViewY * time + 96 > levelHeight * 16) {
		m_viewPoint.y = levelHeight * 16 - 98;
	}
	else {
		m_viewPoint.y += accelerationViewY * time;
	}

	m_viewPoint.y += accelerationViewY * time;
	m_view.setCenter(m_viewPoint.x, m_viewPoint.y);
}

sf::View Player::getView() const
{
	return m_view;
}

void Player::jump()
{
	if (m_onGround)
	{
		m_state = State::JUMP;
		m_velocity.y = -260;
		m_onGround = false;
	}
}

void Player::stay(float time)
{
	if (m_state != State::JUMP) m_velocity.x = 0;
	if (m_state == State::RUN) m_state = State::STAY;
}

void Player::moveLeft(float time)
{
	m_direction = Direction::LEFT;
	
	m_velocity.x = abs(m_velocity.x) < m_maxSpeed ? m_velocity.x - 100 * m_acceleration * time : -m_maxSpeed;
	
	if (m_state == State::STAY)
	{
		m_state = State::RUN;
	}
}

void Player::moveRight(float time)
{
	m_direction = Direction::RIGHT;

	if (m_state == State::STAY)
	{
		m_state = State::RUN;
	}

	m_velocity.x = abs(m_velocity.x) < m_maxSpeed ? m_velocity.x + 100 * m_acceleration * time : m_maxSpeed;
}

bool Player::collisionX(float velocityX)
{
	std::vector<Object> objects = m_level->GetObjects("solid");

	for (const auto& object : objects)
	{
		sf::FloatRect playerRect(m_position.x + velocityX, m_position.y, m_width, m_height);
		sf::FloatRect obstacleRect(object.rect.left, object.rect.top, object.rect.width, object.rect.height);

		if (playerRect.intersects(obstacleRect))
		{
			m_velocity.x = 0;
			return true;
		}
	}
	return false;
}

bool Player::collisionY(float velocityY)
{
	std::vector<Object> objects = m_level->GetObjects("solid");

	for (const auto& object : objects)
	{
		sf::FloatRect playerRect(m_position.x, m_position.y + velocityY, m_width, m_height);
		sf::FloatRect obstacleRect(object.rect.left, object.rect.top, object.rect.width, object.rect.height);

		if (playerRect.intersects(obstacleRect))
		{
			
			bool fromTop = playerRect.top - velocityY + playerRect.height < obstacleRect.top;
			
			if (fromTop)
			{
				m_state = State::STAY;
				m_onGround = true;
			}
			
			m_velocity.y = 0;
			return true;
		}
	}
	return false;
}

bool Player::onGround()
{
	return m_onGround;
}

void Player::update(float time)
{	
	if (m_state == State::STAY)
	{
		m_animationManager.set("stay");
	}

	if (m_state == State::RUN)
	{
		m_animationManager.set("run");
	}

	if (m_state == State::JUMP)
	{
		m_animationManager.set("jump");
	}

	if (m_state == State::ATTACK)
	{
		m_animationManager.set("attack");
		sf::Time attackAnimationTime = sf::seconds(0.3f);
		if (attackClock.getElapsedTime() >= attackAnimationTime)
		{
			m_state = State::STAY;
			m_animationManager.set("stay");
		}
	}

	m_animationManager.flip(m_direction == Direction::LEFT);

	float oldPositionY = m_position.y;

	m_velocity.y += 6.f;

	float velocityY = m_velocity.y * time;

	if (!collisionY(velocityY))
	{
		m_position.y += velocityY;
	}
	else
	{
		m_velocity.y = 0;
	}

	if ((int)m_position.y > (int)oldPositionY + 1)
	{
		m_state = State::JUMP;
	}
	
	float velocityX = m_velocity.x * time;

	if (!collisionX(velocityX))
	{
		m_position.x += velocityX;
	}

	updateView(time);
	m_animationManager.tick(time);
}

void Player::draw(sf::RenderWindow& window)
{
	m_healthBar.draw(window, m_position.x, m_position.y);
	m_animationManager.draw(window, m_position.x, m_position.y);
}

void Player::meleeAttack()
{
	if (attackClock.getElapsedTime() >= attackCooldown)
	{
		m_state = State::ATTACK;
		sf::FloatRect attackArea;
		attackArea.left = m_position.x + (m_direction == Direction::LEFT ? -15.f : 15.f);
		attackArea.top = m_position.y + 8;
		attackArea.width = 18;
		attackArea.height = 16;

		std::vector<std::shared_ptr<Enemy>> enemiesInAttackArea = m_level->GetEnemiesInArea(attackArea);

		for (std::shared_ptr<Enemy> enemy : enemiesInAttackArea)
		{
			enemy->takeDamage(25 + rand() % 10, m_direction == Direction::RIGHT);
		}

		attackClock.restart();
	}
}

float Player::getX()
{
	return m_position.x;
}

float Player::getY()
{
	return m_position.y;
}

float Player::getWidth()
{
	return m_width;
}

float Player::getHeight()
{
	return m_height;
}

void Player::takeDamage(int damage, bool damageFromRight)
{
	m_health = m_health - damage;
	m_healthBar.setHealth(m_health);

	float knockbackDistance = 10.0f;
	float newX = m_position.x;

	if (!collisionX((damageFromRight) ? knockbackDistance : -knockbackDistance))
	{
		newX += (damageFromRight) ? knockbackDistance : -knockbackDistance;
	}

	if (m_health <= 0)
	{
		m_level->setGameOver(true);
	}

	m_position.x = newX;
}

void Player::drawScore(sf::RenderWindow& window, int score) const {
	sf::Font font;
	if (!font.loadFromFile("./assets/arial.ttf")) {
		return;
	}

	sf::Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(12);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setString("Score: " + std::to_string(score));
	scoreText.setPosition(m_view.getCenter().x - 124, m_view.getCenter().y - 96);

	window.draw(scoreText);
}