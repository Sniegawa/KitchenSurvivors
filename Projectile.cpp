#include "Projectile.h"

Projectile::Projectile(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, float rotation, float speed, glm::vec2 velocity)
{
	this->Position = pos;
	this->Size = size;
	this->Sprite = sprite;
	this->Color = color;
	this->Rotation = rotation;
	this->Speed = speed;
	this->Velocity = velocity;
}

void Projectile::UpdatePosition(float dt)
{
	this->Position += this->Velocity * dt * Speed;
}