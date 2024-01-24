#pragma once
#include <map>
#include <string>
#include <iostream>
#include "Animation.h"

class AnimationManager
{
public:
	AnimationManager() {};
	void create(std::string name, sf::Texture& texture, int x, int y, int w, int h, int count, float speed, int step);
	void draw(sf::RenderWindow& window, int x, int y);
	void set(const std::string& name);
	void flip(const bool isFlip);
	void tick(const float time);
	void pause();
	void play();

private:
	std::string m_currentAnimationName;
	std::map<std::string, Animation> m_animations;
};
