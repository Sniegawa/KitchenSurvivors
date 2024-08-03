#pragma once

#include "SpriteRenderer.h"


class EnemyRenderer : public SpriteRenderer
{
public:
	glm::vec2* PlayerPosition;

	EnemyRenderer(const SpriteRenderer& sr, glm::vec2* PlayerPos);

	void DrawSprite(Texture2D texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
};