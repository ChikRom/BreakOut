#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include <GLAD/glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

#include "shader.h"
#include "texture.h"
#include "game_object.h"

// Represents a single particle and its state
struct Particle
{
	glm::vec2	Position, Velocity;
	glm::vec4	Colour;
	float		Life;

	Particle() : Position(0.0f), Velocity(0.0f), Colour(1.0f), Life(0.0f) { }
};

// ParticleGenerator acts as a container for rendering a large number of particles
// by repeatedly spawning and updating particles and killing them after a given
// amount of time
class ParticleGenerator
{
public:
	// constructor
	ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
	// update all particles
	void Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f,0.0f));
	// render all particles
	void Draw();
private:
	// state
	std::vector<Particle> particles;
	unsigned int amount;
	// render state
	Shader shader;
	Texture2D texture;
	unsigned int VAO;
	// initializes buffer and vertex attributes
	void init();
	// returns the first Particle index that's currently unused (Life <= 0.0f or 0 index)
	unsigned int firstUnusedParticle();
	// respanws particle
	void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif