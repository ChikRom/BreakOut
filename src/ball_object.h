#ifndef BALLOBJECT_H
#define BALLOBJECT_H



#include "game_object.h"


class BallObject : public GameObject
{
public:
	// ball state
	float Radius;
	bool Stuck;
	// constructor(s)
	BallObject();
	BallObject(glm::vec2 pos, glm::vec2 velocity, float radius, Texture2D sprite);
	// moves the ball, keeping it constrained within the window bounds
	// (except bottom edge) return new position
	glm::vec2 Move(float dt, unsigned int window_width);
	// reset the ball to original state with given position and velocity
	void Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif