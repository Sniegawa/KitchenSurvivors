#include "GameObject.h"

GameObject::GameObject()
	: m_Position(0.0f,0.0f), m_Size(1.0f,1.0f), m_Color(1.0f), m_Rotation(0.0f), m_Sprite(), m_IsStatic(false) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, float rotation, Texture2D* sprite, Shader* _shader, RenderLayer layer, glm::vec3 color, bool isstatic)
	: m_Position(pos), m_Size(size), m_Color(color), m_Rotation(rotation), m_Sprite(sprite), m_shader(_shader), m_renderLayer(layer), m_IsStatic(isstatic) 
{
	this->m_Moved = true; //Set to true because if it's false CalculateModelMatrix() won't calculate anything
	this->CalculateModelMatrix();
}


void GameObject::SetRenderLayer(RenderLayer layer)
{
	this->m_renderLayer = layer;
}

glm::mat4 GameObject::CalculateModelMatrix()
{
	if (!this->m_Moved || this->m_IsStatic)
	{
		return this->m_ModelMatrix;
	}
	glm::mat4 model = glm::mat4(1.0f);
	//Position
	model = glm::translate(model, glm::vec3(this->m_Position, 0.0f));

	//Rotation
	model = glm::translate(model, glm::vec3(0.5f * this->m_Size.x, 0.5f * this->m_Size.y, 0.0f)); //We transform model to be centered at center of object
	model = glm::rotate(model, this->m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f)); //Rotate
	model = glm::translate(model, glm::vec3(-0.5f * this->m_Size.x, -0.5f * this->m_Size.y, 0.0f)); //transform model so it's center is back at top left corner
	//Scale
	model = glm::scale(model, glm::vec3(this->m_Size, 1.0f));

	this->m_ModelMatrix = model;
	this->m_Moved = false;

	return model;
}