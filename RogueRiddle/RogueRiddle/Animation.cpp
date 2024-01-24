#include "Animation.h"
#include <iostream>

Animation::Animation(sf::Texture& texture, int x, int y, int w, int h, int countFrames, float speed, int step)
{
	m_speed = speed;
	m_sprite.setTexture(texture);
	m_currentFrame = 0;
	m_isPlaying = true;
	m_isFlip = false;

	for (int i = 0; i < countFrames; i++)
	{
		m_frames.push_back(sf::IntRect(x + i * step, y, w, h));
		m_framesFlip.push_back(sf::IntRect(x + i * step + w, y, -w, h));
	}
}

void Animation::tick(float time)
{
	if (!m_isPlaying)
	{
		return;
	}
	
	m_currentFrame += m_speed * time;

	if (m_currentFrame >= m_frames.size())
	{
		m_currentFrame = 0;
	}

	if (m_isFlip)
	{
		m_sprite.setTextureRect(m_framesFlip[m_currentFrame]);
	}
	else
	{
		m_sprite.setTextureRect(m_frames[m_currentFrame]);
	}
}

sf::Sprite Animation::getSprite() const
{
	return m_sprite;
}

void Animation::setFlip(const bool isFlip)
{
	m_isFlip = isFlip;
}

void Animation::setIsPlaying(bool isPlaying)
{
	m_isPlaying = isPlaying;
}
