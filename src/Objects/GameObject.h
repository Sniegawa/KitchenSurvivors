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

	
	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size,float rotation, Texture2D* sprite, Shader* shader,RenderLayer layer, glm::vec3 color = glm::vec3(1.0f));

	void SetRenderLayer(RenderLayer layer);

	const RenderLayer& GetRenderLayer() const { return this->m_renderLayer; }
	
	const glm::vec2& GetPosition() const { return this->m_Position; }
	void SetPosition(glm::vec2 pos) { this->m_Position = pos; }
	void Move(glm::vec2 pos) { this->m_Position += pos; }

	const glm::vec2& GetSize() const { return this->m_Size; }
	void SetSize(glm::vec2 size) { this->m_Size = size; }

	const glm::vec3& GetColor() const { return this->m_Color; }
	void SetColor(glm::vec3 color) { this->m_Color = color; }

	const float& GetRotation() const { return this->m_Rotation; }
	void SetRotation(float rotation) { this->m_Rotation = rotation; }
	void RotateByAngle(float amount) { this->m_Rotation += amount; }

	Texture2D* GetSprite()  { return this->m_Sprite; }
	void SetSprite(Texture2D* sprite) { this->m_Sprite = sprite; }

	Shader* GetShader()  { return this->m_shader; }
	void SetShader(Shader* shader) { this->m_shader = shader; }

	glm::mat4 CalculateModelMatrix();

protected:
	glm::vec2 m_Position;
	glm::vec2 m_Size;
	glm::vec3 m_Color;
	float m_Rotation;

	Texture2D* m_Sprite;
	Shader* m_shader;

	RenderLayer m_renderLayer;
};