#pragma once

#include <GL/glew.h>
#include <glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"

class GameObject
{
public:
	glm::vec2 Position, Size;
	glm::vec3 Color;
	float Rotation;
	bool IsSolid;

	Texture2D Sprite;

	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f),float rotation = 0.0f);

	virtual void Draw(SpriteRenderer& renderer);
	virtual void Draw(SpriteRenderer& renderer, glm::vec2 PlayerPosition);
};