#include "game_object.h"

GameObject::GameObject()
	: Position(0.0f,0.0f), Size(1.0f,1.0f), Colour(1.0f), Velocity(0.0f,0.0f), Rotation(0.0f), 
	  Sprite(), IsSolid(false), Destroyed(false)
{
}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 colour, glm::vec2 velocity)
	: Position(pos), Size(size), Sprite(sprite), Colour(colour), Velocity(velocity), Rotation(0.0f),
	  IsSolid(false), Destroyed(false)
{
}

void GameObject::Draw(SpriteRenderer& renderer)
{
	renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Colour);
}
