#pragma once
#include <SFML/Graphics.hpp>
#include "AnimationManager.h"

const enum class Direction {
	LEFT,
	RIGHT,
};

const enum class State {
	STAY,
	RUN,
	JUMP,
	ATTACK,
};

class IEntity
{
	virtual void update(float time) = 0;
	virtual void draw(sf::RenderWindow& windpow) = 0;
};