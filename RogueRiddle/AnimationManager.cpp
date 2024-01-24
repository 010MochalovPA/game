#include "AnimationManager.h"

void AnimationManager::create(std::string name, sf::Texture& texture, int x, int y, int w, int h, int count, float speed, int step)
{
	m_animations[name] = Animation(texture, x, y, w, h, count, speed, step);
	m_currentAnimationName = name;
}

void AnimationManager::draw(sf::RenderWindow& window, int x, int y)
{
	sf::Sprite sprite = m_animations[m_currentAnimationName].getSprite();
	sprite.setPosition(x, y);
	window.draw(sprite);
}

void AnimationManager::set(const std::string& name)
{
	m_currentAnimationName = name;
}

void AnimationManager::flip(const bool isFlip)
{
	m_animations[m_currentAnimationName].setFlip(isFlip);
}

void AnimationManager::tick(const float time)
{
	m_animations[m_currentAnimationName].tick(time);
}

void AnimationManager::pause()
{
	m_animations[m_currentAnimationName].setIsPlaying(false);
}

void AnimationManager::play()
{
	m_animations[m_currentAnimationName].setIsPlaying(true);
}