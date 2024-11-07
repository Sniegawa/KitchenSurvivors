#pragma once

#include <GL/glew.h>
#include <glm.hpp>

#include "../ResourceHandlers/Texture.h"
#include "../Renderers/SpriteRenderer.h"

class GameObject
{
public:
	glm::vec2 Position, Size;
	glm::vec3 Color;
	float Rotation;
	bool IsSolid;

	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size,float rotation, Texture2D* sprite, glm::vec3 color = glm::vec3(1.0f));

	virtual void Draw(SpriteRenderer& renderer);
	virtual void Draw(SpriteRenderer& renderer, glm::vec2 PlayerPosition);
	Texture2D* Sprite;
private:
};