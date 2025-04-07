#include "GameObject.h"

GameObject::GameObject()
	: m_Position(0.0f,0.0f), m_Size(1.0f,1.0f), m_Color(1.0f), m_Rotation(0.0f), m_Sprite() { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, float rotation, Texture2D* sprite, Shader* _shader, RenderLayer layer, glm::vec3 color)
	: m_Position(pos), m_Size(size), m_Color(color), m_Rotation(rotation), m_Sprite(sprite), m_shader(_shader), m_renderLayer(layer) { }


void GameObject::SetRenderLayer(RenderLayer layer)
{
	this->m_renderLayer = layer;
}