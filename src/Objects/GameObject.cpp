#include "GameObject.h"

GameObject::GameObject()
	: Position(0.0f,0.0f), Size(1.0f,1.0f), Color(1.0f), Rotation(0.0f), Sprite() { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, float rotation, Texture2D* sprite, Shader* _shader, RenderLayer layer, glm::vec3 color)
	: Position(pos), Size(size), Color(color), Rotation(rotation), Sprite(sprite), shader(_shader), renderLayer(layer) { }


void GameObject::SetRenderLayer(RenderLayer layer)
{
	this->renderLayer = layer;
}