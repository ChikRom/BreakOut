#ifndef POWER_UP_H
#define POWER_UP_H

#include <string>

#include <glm/glm.hpp>
#include "game_object.h"

// The size of a PowerUp block
const glm::vec2 POWERUP_SIZE(120.f, 40.0f);
// Velocity a PowerUp block has when spawned
const glm::vec2 VELOCITY(0.0f, 300.0f);

// PowerUp inherits its state and rendering functions from
// GameObject but also holds extra information to state its
// active duration and whether it is activated or not.
// The type of PowerUp is stored as a string.
class PowerUP : public GameObject
{
public:
	// powerup state
	std::string Type;
	float		Duration;
	bool		Activated;
	// constructor
	PowerUP(std::string type, glm::vec3 colour, float duration, glm::vec2 position, Texture2D texture)
		: GameObject(position, POWERUP_SIZE, texture, colour, VELOCITY), Type(type), Duration(duration), Activated(false) {}
};

#endif