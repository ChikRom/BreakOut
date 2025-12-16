#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "ball_object.h"

// Game-related State data
SpriteRenderer* Renderer;
GameObject*		Player;
BallObject*		Ball;

Direction VectorDirection(glm::vec2 target);

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
}


Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
}

void Game::Init()
{
	// load shaders
	ResourceManager::LoadShader("shaders/sprite.vert", "shaders/sprite.frag",nullptr,"sprite");
	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), 
		static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use();
	ResourceManager::GetShader("sprite").setInt("image", 0);
	ResourceManager::GetShader("sprite").setMat4("projection", projection);
	// set render specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	// load textures
	ResourceManager::LoadTexture("textures/awesomeface.png", true, "face");
	ResourceManager::LoadTexture("textures/block.png", false, "block");
	ResourceManager::LoadTexture("textures/block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("textures/background.jpg", false, "background");
	ResourceManager::LoadTexture("textures/paddle.png", true, "player");
	// load levels
	GameLevel standard; standard.Load("levels/standard.lvl", this->Width, this->Height / 2.0f);

	this->Levels.push_back(standard);
	this->Level = 0;

	//configure game objects
	glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
		this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("player"));

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);

	Ball = new BallObject(ballPos, BALL_VELOCITY, BALL_RADIUS, ResourceManager::GetTexture("face"));
}

void Game::ProcessInput(float dt)
{
	if (this->State == GAME_ACTIVE)
	{
		float velocity = PLAYER_VELOCITY * dt;
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0.0f)
			{
				Player->Position.x -= velocity;
				if (Ball->Stuck)
					Ball->Position.x -= velocity;
			}
		}
		if (this->Keys[GLFW_KEY_D])
			if (Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
				if (Ball->Stuck)
					Ball->Position.x += velocity;
			}
		if (this->Keys[GLFW_KEY_SPACE])
			Ball->Stuck = false;
	}
}

void Game::Update(float dt)
{
	// update objects
	Ball->Move(dt, this->Width);
	// check for collisions
	this->DoCollisions();
	// check loss condition
	if (Ball->Position.y >= this->Height) // did ball reach the bottom edge?
	{
		this->ResetLevel();
		this->ResetPlayer();
	}
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		// draw background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f),
			glm::vec2(this->Width, this->Height));
		// draw level
		this->Levels[this->Level].Draw(*Renderer);
		// draw player
		Player->Draw(*Renderer);
		// draw ball
		Ball->Draw(*Renderer);
	}
	/*Renderer->DrawSprite(ResourceManager::GetTexture("face"), glm::vec2(600.0f, 400.0f),
		glm::vec2(600.f, 800.f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));*/
}


bool Game::CheckCollision(GameObject& one, GameObject& two)
{
	// collision x-axis
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
		two.Position.x + two.Size.x >= one.Position.x;
	// collision y-axis
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		two.Position.y + two.Size.y >= one.Position.y;
	return collisionX && collisionY;
}


Collision Game::CheckCollision(BallObject& ball, GameObject& obj)
{
	// calculate the centers of shapes
	glm::vec2 circleCenter(ball.Position + ball.Radius);
	glm::vec2 aabb_half_extents(obj.Size.x / 2.0f, obj.Size.y / 2.0f);
	glm::vec2 aabb_center(obj.Position + aabb_half_extents);
	// get difference vector between both centers
	glm::vec2 difference = circleCenter - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// closet position to circle
	glm::vec2 closet = aabb_center + clamped;
	// vector between radius of circle and closet position to circle
	difference = closet - circleCenter;
	if (glm::length(difference) < ball.Radius)
		return std::make_tuple(true, VectorDirection(difference), difference);

	return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

void Game::DoCollisions()
{
	for (GameObject& box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision)) // if collision is true
			{
				// destroy block if not solid
				if (!box.IsSolid)
					box.Destroyed = true;
				// collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vect = std::get<2>(collision);
				if (dir == LEFT || dir == RIGHT) // horizontal collision
				{
					Ball->Velocity.x *= -1; // reverse horizontal velocity
					// relocate
					float penetration = Ball->Radius - std::abs(diff_vect.x);
					if (dir == LEFT)
						Ball->Position.x += penetration; // to right
					else
						Ball->Position.x -= penetration; // to left
				}
				else
				{
					Ball->Velocity.y *= -1; // reverse vertical velocity
					// relocate
					float penetration = Ball->Radius - std::abs(diff_vect.y);
					if (dir == UP)
						Ball->Position.y -= penetration; // back up
					else
						Ball->Position.y += penetration; // back down
				}
			}
		}
	}

	// check collisions for plaeyr pad (unless stuck)
	Collision result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		// check where it hit the board, and change velocity based on where it hit the board
		float centerBoardX = Player->Position.x + Player->Size.x / 2.0f;
		float distance = (Ball->Position.x + Ball->Radius) - centerBoardX;
		float percentage = distance / (Player->Size.x / 2.0f);
		// then move accordingly
		float strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = BALL_VELOCITY.x * percentage * strength;
		// keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
		// fix sticky paddle
		Ball->Velocity.y = -1.0f * std::abs(Ball->Velocity.y);

	}
}

void Game::ResetLevel()
{
	if (this->Level == 0)
		this->Levels[Level].Load("levels/standard.lvl", this->Width, this->Height/2);
}

void Game::ResetPlayer()
{
	// reset player/ball state
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -2.0f * BALL_RADIUS), BALL_VELOCITY);

}

// calculates which direction a vector is facing
Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] =
	{
		glm::vec2(0.0f,1.0f), // UP
		glm::vec2(1.0f,0.0f), // RIGHT
		glm::vec2(0.0f,-1.0f), // DOWN
		glm::vec2(-1.0f,0.0f) // LEFT
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++)
	{
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}