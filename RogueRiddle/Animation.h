#pragma once
#include <SFML/Graphics.hpp>

class Animation
{
public:
	Animation() = default;
	Animation(sf::Texture& texture, int x, int y, int w, int h, int countFrames, float speed, int step);
	void tick(float time);
	sf::Sprite getSprite() const;
	void setFlip(bool isFlip);
	void setIsPlaying(bool isPlaying);

private:
	std::vector<sf::IntRect> m_frames;
	std::vector<sf::IntRect> m_framesFlip;
	float m_currentFrame;
	float m_speed;
	bool m_isFlip;
	bool m_isPlaying;
	sf::Sprite m_sprite;
};