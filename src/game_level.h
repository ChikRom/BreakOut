#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <vector>

#include <GLAD/glad/glad.h>
#include <glm/glm.hpp>

#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"

// GameLevel holds all Tiles as part of a Breakout level and
// hosts functionality to Load/render levels from the harddisk.
class GameLevel
{
public:
	// level state
	std::vector<GameObject> Bricks;
	// loads level from file
	void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);
	// render level
	void Draw(SpriteRenderer& renderer);
	// check if the level is completed (all non-solid tiles are destroyed)
	bool isCompleted();
private:
	// initialize level from tile data
	void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif
