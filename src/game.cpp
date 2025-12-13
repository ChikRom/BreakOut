#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"

// Game-related State data
SpriteRenderer *Renderer;
GameObject* Player;

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(200.0f, 50.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(1000.0f);

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
}


Game::~Game()
{
	delete Renderer;
	delete Player;
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
}

void Game::ProcessInput(float dt)
{
	if (this->State == GAME_ACTIVE)
	{
		float velocity = PLAYER_VELOCITY * dt;
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0.0f)
				Player->Position.x -= velocity;
		}
		if (this->Keys[GLFW_KEY_D])
			if (Player->Position.x <= this->Width - Player->Size.x)
				Player->Position.x += velocity;
	}
}

void Game::Update(float dt)
{
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
	}
	/*Renderer->DrawSprite(ResourceManager::GetTexture("face"), glm::vec2(600.0f, 400.0f),
		glm::vec2(600.f, 800.f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));*/
}
