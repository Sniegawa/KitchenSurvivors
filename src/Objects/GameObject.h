#pragma once

#include <GL/glew.h>
#include <glm.hpp>

#include "../ResourceHandlers/Texture.h"
#include "../ResourceHandlers/ShaderLoader.h"
enum RenderLayer {
	BACKGROUND,
	ENEMY,
	PLAYER,
	PROJECTILES,
	PICKUPS
};

class GameObject
{
public:
	glm::vec2 Position, Size;
	glm::vec3 Color;
	float Rotation;
	
	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size,float rotation, Texture2D* sprite, Shader* shader,RenderLayer layer, glm::vec3 color = glm::vec3(1.0f));

	void SetRenderLayer(RenderLayer layer);

	const RenderLayer GetRenderLayer() { return this->renderLayer; };

	Texture2D* Sprite;
	Shader* shader;
private:
	RenderLayer renderLayer;
};