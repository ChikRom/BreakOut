#ifndef GAME_H
#define GAME_H

#include <GLAD/glad/glad.h>
#include <GLFW/glfw3.h>
#include "ball_object.h"
#include "game_level.h"

// current state of the game
enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

// possible collision direction
enum Direction
{
	UP,
	RIGHT,
	DOWN,
	LEFT
};

// defines a Collision typedef that represents collision data
typedef std::tuple<bool, Direction, glm::vec2> Collision; //(collision?,direction, center - closet point)

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(200.0f, 50.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(1000.0f);

// Initial radius of the Ball
const float BALL_RADIUS = 25.0f;
// Initial velocity of the Ball
const glm::vec2 BALL_VELOCITY(300.0f, -950.0f);

class Game
{
public:
	// levels
	std::vector<GameLevel>	Levels;
	unsigned int			Level;
	// game state
	GameState				State;
	bool					Keys[1024];
	unsigned int			Width, Height;
	// constructor/destructor
	Game(unsigned int width, unsigned int height);
	~Game();
	// initialize game state (load all shaders/textures/levels)
	void Init();
	// game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
	// check collisions
	bool CheckCollision(GameObject& one, GameObject& two); // (axis-aligned box bounding algorithm)
	Collision CheckCollision(BallObject& ball, GameObject& obj); // (algorithm between circle and rectangle)
	void DoCollisions();
	// reset
	void ResetLevel();
	void ResetPlayer();
};

#endif GAME_H
