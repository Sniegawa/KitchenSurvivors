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
	GameObject(glm::vec2 pos, glm::vec2 size,float rotation, Texture2D* sprite, Shader* shader,RenderLayer layer, glm::vec3 color = glm::vec3(1.0f), bool isstatic = false);

	void SetRenderLayer(RenderLayer layer);

	const RenderLayer& GetRenderLayer() const { return this->m_renderLayer; }
	
	const glm::vec2& GetPosition() const { return this->m_Position; }
	void SetPosition(glm::vec2 pos) { this->m_Position = pos; this->m_Moved = true; }
	void Move(glm::vec2 pos) { this->m_Position += pos; this->m_Moved = true; }

	const glm::vec2& GetSize() const { return this->m_Size; }
	void SetSize(glm::vec2 size) { this->m_Size = size; this->m_Moved = true; }

	const glm::vec3& GetColor() const { return this->m_Color; }
	void SetColor(glm::vec3 color) { this->m_Color = color; }

	const float& GetRotation() const { return this->m_Rotation; }
	void SetRotation(float rotation) { this->m_Rotation = rotation; this->m_Moved = true; }
	void RotateByAngle(float amount) { this->m_Rotation += amount; this->m_Moved = true; }

	Texture2D* GetSprite()  { return this->m_Sprite; }
	void SetSprite(Texture2D* sprite) { this->m_Sprite = sprite; }

	Shader* GetShader()  { return this->m_shader; }
	void SetShader(Shader* shader) { this->m_shader = shader; }

	glm::mat4 CalculateModelMatrix();

	const bool& IsStatic() const { return this->m_IsStatic; }

protected:
	glm::vec2 m_Position;
	glm::vec2 m_Size;
	glm::vec3 m_Color;
	
	//If object have been moved before rendering frame we need to recalculate our model matrix
	bool m_Moved;

	glm::mat4 m_ModelMatrix;

	float m_Rotation;

	bool m_IsStatic;

	Texture2D* m_Sprite;
	Shader* m_shader;

	RenderLayer m_renderLayer;
};