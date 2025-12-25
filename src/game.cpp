#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "ball_object.h"
#include "particle_generator.h"
#include "post_processor.h"
#include <irrklang/irrKlang.h>
#include <algorithm>
#include "text_renderer.h"
using namespace irrklang;



// Game-related State data
SpriteRenderer*		Renderer;
GameObject*			Player;
BallObject*			Ball;
ParticleGenerator*	Particles;
PostProcessor*		Effects;
ISoundEngine*		SoundEngine = createIrrKlangDevice();
TextRenderer*		Text;

float ShakeTime = 0.0f;

Direction VectorDirection(glm::vec2 target);

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_MENU), Keys(), Width(width), Height(height), Level(0), Lives(3)
{
}


Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete Effects;
	delete Text;
	SoundEngine->drop();
}

void Game::Init()
{
	// load audio
	SoundEngine->play2D("audio/breakout.mp3", true);
	// load shaders
	ResourceManager::LoadShader("shaders/sprite.vert", "shaders/sprite.frag",nullptr,"sprite");
	ResourceManager::LoadShader("shaders/particle.vert", "shaders/particle.frag",nullptr,"particle");
	ResourceManager::LoadShader("shaders/post_processing.vert", "shaders/post_processing.frag",nullptr,"postprocessing");
	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), 
		static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use();
	ResourceManager::GetShader("sprite").setInt("image", 0);
	ResourceManager::GetShader("sprite").setMat4("projection", projection);
	ResourceManager::GetShader("particle").Use();
	ResourceManager::GetShader("particle").setInt("sprite", 0);
	ResourceManager::GetShader("particle").setMat4("projection", projection);
	// load textures
	ResourceManager::LoadTexture("textures/awesomeface.png", true, "face");
	ResourceManager::LoadTexture("textures/block.png", false, "block");
	ResourceManager::LoadTexture("textures/block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("textures/background.jpg", false, "background");
	ResourceManager::LoadTexture("textures/paddle.png", true, "player");
	ResourceManager::LoadTexture("textures/particle.png", true, "particle");
	ResourceManager::LoadTexture("textures/powerup_speed.png", true, "powerup_speed");
	ResourceManager::LoadTexture("textures/powerup_confuse.png", true, "powerup_confuse");
	ResourceManager::LoadTexture("textures/powerup_chaos.png", true, "powerup_chaos");
	ResourceManager::LoadTexture("textures/powerup_increase.png", true, "powerup_increase");
	ResourceManager::LoadTexture("textures/powerup_passthrough.png", true, "powerup_passthrough");
	ResourceManager::LoadTexture("textures/powerup_sticky.png", true, "powerup_sticky");
	// set render specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 2000);
	Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("resources/fonts/times.ttf",90);
	// load levels
	GameLevel standard; standard.Load("levels/standard.lvl", this->Width, this->Height / 2.0f);
	GameLevel two; two.Load("levels/level_two.lvl", this->Width, this->Height / 2.0f);
	GameLevel three; three.Load("levels/level_three.lvl", this->Width, this->Height / 2.0f);
	GameLevel four; four.Load("levels/level_four.lvl", this->Width, this->Height / 2.0f);

	this->Levels.push_back(standard);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 0;

	//configure game objects
	glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);

	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("player"));

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);

	Ball = new BallObject(ballPos, BALL_VELOCITY, BALL_RADIUS, ResourceManager::GetTexture("face"));
}

void Game::ProcessInput(float dt)
{
	if (this->State == GAME_MENU)
	{
		if (this->Keys[GLFW_KEY_SPACE] && !this->KeysProcessed[GLFW_KEY_SPACE])
		{
			this->State = GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_SPACE] = true;
		}
		if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
		{
			this->Level = (this->Level + 1) % 4;
			this->KeysProcessed[GLFW_KEY_W] = true;
		}
		if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
		{
			if (this->Level > 0)
				--this->Level;
			else
				this->Level = 3;
			this->KeysProcessed[GLFW_KEY_S] = true;
		}
	}
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
	if (this->State == GAME_WIN)
	{
		if (this->Keys[GLFW_KEY_LEFT_ALT])
		{
			this->KeysProcessed[GLFW_KEY_LEFT_ALT] = true;
			Effects->Chaos = false;
			this->State = GAME_MENU;
		}
	}
}

void Game::Update(float dt)
{
	// update objects
	Ball->Move(dt, this->Width);
	// check for collisions
	this->DoCollisions();
	// update particle system
	Particles->Update(dt, *Ball, 4, glm::vec2(Ball->Radius / 2.0f));
	// update PowerUps
	this->UpdatePowerUps(dt);
	// reduce shake time
	if (ShakeTime > 0.0f)
	{
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			Effects->Shake = false;
	}
	// check loss condition
	if (Ball->Position.y >= this->Height) // did ball reach the bottom edge?
	{
		--this->Lives;
		// did the player lose all his lives? : Game over
		if (this->Lives == 0)
		{
			this->ResetLevel();
			this->State = GAME_MENU;
		}
		this->ResetPlayer();
	}
	if (this->State == GAME_ACTIVE && this->Levels[this->Level].isCompleted())
	{
		this->ResetLevel();
		this->ResetPlayer();
		Effects->Chaos = true;
		this->State = GAME_WIN;
	}
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN)
	{
		// begin rendering to postprocessing framebuffer
		Effects->BeginRender();
		// draw background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f),
			glm::vec2(this->Width, this->Height));
		// draw level
		this->Levels[this->Level].Draw(*Renderer);
		// draw player
		Player->Draw(*Renderer);
		// draw powerUps
		for (PowerUP& powerUp : this->PowerUps)
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);
		// draw particles
		if (!Ball->Stuck)
			Particles->Draw();
		// draw ball
		Ball->Draw(*Renderer);
		// end rendering to postprocessing framebuffer
		Effects->EndRender();
		// render postprocessing quad
		Effects->Render(glfwGetTime());
		// render text (don't include in postprocessing)
		std::stringstream ss1, ss2; ss1 << this->Lives; ss2 << this->Level;
		Text->RenderText("Lives: " + ss1.str(), 15.0f, 15.0f, 1.0f);
		Text->RenderText("Level: " + ss2.str(), 2100.0f, 15.0f, 1.0f);
	}
	if (this->State == GAME_MENU)
	{
		Text->RenderText("Press SPACE to start", 900.0f, this->Height / 2.0f + 5.0f, 0.8f);
		Text->RenderText("Press W or S to select level", 800.0f, this->Height / 2.0f + 80.0f, 0.8f);
	}
	if (this->State == GAME_WIN)
	{
		Text->RenderText("You WON", 900.0f, this->Height / 2.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		Text->RenderText("Press LEFT_ALT to retry or ESC to quit", 400.0f, this->Height / 2.0f + 75.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
	}
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

void ActivatePowerUp(PowerUP& powerUP)
{
	if (powerUP.Type == "speed")
	{
		Ball->Velocity *= 1.2f;
	}
	else if (powerUP.Type == "sticky")
	{
		Ball->Sticky = true;
		Player->Colour = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUP.Type == "pass_through")
	{
		Ball->PassThrough = true;
		Ball->Colour = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUP.Type == "pad-size-increase")
	{
		Player->Size.x += 100.0f;
	}
	else if (powerUP.Type == "confuse")
	{
		Effects->Chaos = false;
		Effects->Confuse = true; // first - disable chaos, than confuse
	}
	else if (powerUP.Type == "chaos")
	{
		Effects->Confuse = false;
		Effects->Chaos = true; // first - disable confuse, than chaos
	}

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
				{
					SoundEngine->play2D("audio/bleep.mp3", false);
					box.Destroyed = true;
					this->SpawnPowerUps(box);
				}
				else
				{
					SoundEngine->play2D("audio/solid.wav", false);
					// if block is solid, enable shake effect
					ShakeTime = 0.05f;
					Effects->Shake = true;
				}
				// collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vect = std::get<2>(collision);
				if (!Ball->PassThrough || box.IsSolid)
				{
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
	}

	// also check collisions on PowerUps and if so, activate them 
	for (PowerUP& powerUP : this->PowerUps)
	{
		if (!powerUP.Destroyed)
		{
			if (powerUP.Position.y >= this->Height)
				powerUP.Destroyed = true;
			if (this->CheckCollision(*Player, powerUP))
			{
				SoundEngine->play2D("audio/powerup.wav", false);
				ActivatePowerUp(powerUP);
				powerUP.Activated = true;
				powerUP.Destroyed = true;
			}
		}
	}

	// check collisions for player pad (unless stuck)
	Collision result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		SoundEngine->play2D("audio/bleep.wav", false);
		// check where it hit the board, and change velocity based on where it hit the board
		float centerBoardX = Player->Position.x + Player->Size.x / 2.0f;
		float distance = (Ball->Position.x + Ball->Radius) - centerBoardX;
		float percentage = distance / (Player->Size.x / 2.0f);
		// then move accordingly
		float strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = (BALL_VELOCITY.x + 300.0f) * percentage * strength;
		// keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
		// fix sticky paddle
		Ball->Velocity.y = -1.0f * std::abs(Ball->Velocity.y);

		// if Sticky powerup is activated, also stick ball to paddle once new velocity velocity vectors
		// were calculated
		Ball->Stuck = Ball->Sticky;
	}
}

void Game::ResetLevel()
{
	if (this->Level == 0)
		this->Levels[Level].Load("levels/standard.lvl", this->Width, this->Height/2);
	if (this->Level == 1)
		this->Levels[Level].Load("levels/level_two.lvl", this->Width, this->Height / 2);
	if (this->Level == 2)
		this->Levels[Level].Load("levels/level_three.lvl", this->Width, this->Height / 2);
	if (this->Level == 3)
		this->Levels[Level].Load("levels/level_four.lvl", this->Width, this->Height / 2);
	this->Lives = 3;
}

void Game::ResetPlayer()
{
	// reset player/ball state
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -2.0f * BALL_RADIUS), BALL_VELOCITY);

	Effects->Chaos = Effects->Confuse = false;
	Ball->PassThrough = Ball->Sticky = false;
	Ball->Colour = glm::vec3(1.0f);
	Player->Colour = glm::vec3(1.0f);

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

bool ShouldSpawn(unsigned int chance)
{
	unsigned int random = rand() % chance;
	return random == 0;
}

void Game::SpawnPowerUps(GameObject& block)
{
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUP("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUP("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUP("pass_through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUP("pad-size-increase", glm::vec3(1.0f,0.6f, 0.4f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
	if (ShouldSpawn(15))
		this->PowerUps.push_back(PowerUP("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
	if (ShouldSpawn(15))
		this->PowerUps.push_back(PowerUP("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));
}

bool isOtherPowerUPActive(std::vector<PowerUP>& powerUps, std::string type)
{
	// Check if another PowerUp of the other type is still active
	// in which case we don't disable its effect (yet)
	for (const PowerUP& powerUp : powerUps)
	{
		if (powerUp.Activated && powerUp.Type == type)
			return true;
	}
	return false;
}

void Game::UpdatePowerUps(float dt)
{
	for (PowerUP& powerUp : this->PowerUps)
	{
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;
			if (powerUp.Duration <= 0.0f)
			{
				// remove powerup from list (will later be removed)
				powerUp.Activated = false;
				// deactivate effects
				if (powerUp.Type == "sticky" && !isOtherPowerUPActive(this->PowerUps, "sticky"))
				{
					Ball->Sticky = false;
					Player->Colour = glm::vec3(1.0f);
				}
				else if (powerUp.Type == "pass_through" && !isOtherPowerUPActive(this->PowerUps, "pass_through"))
				{
					Ball->PassThrough = false;
					Ball->Colour = glm::vec3(1.0f);
				}
				else if (powerUp.Type == "confuse" && !isOtherPowerUPActive(this->PowerUps, "confuse"))
				{
					Effects->Confuse = false;
				}
				else if (powerUp.Type == "chaos" && !isOtherPowerUPActive(this->PowerUps, "chaos"))
				{
					Effects->Chaos = false;
				}
			}
		}
	}
	// Remove all PowerUps from vector that are destroyed AND !activated (thus either off the map or finished)
	// Use a lambda expression to remove each PowerUp which is destroyed and not activated
	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const PowerUP& powerUp) {return powerUp.Destroyed && !powerUp.Activated; }
	), this->PowerUps.end());
}