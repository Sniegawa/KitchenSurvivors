#pragma once

#include "../ResourceHandlers/ResourceManager.h"

class SpriteRenderer
{
public:
	SpriteRenderer(Shader& shader);
	virtual ~SpriteRenderer();
	//glm::vec2* PlayerPosition;

	virtual void DrawSprite(Texture2D texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
protected:
	Shader shader;
	unsigned int quadVAO;

	void initRenderData();

};