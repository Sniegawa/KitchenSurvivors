#include "Projectile.h"

Projectile::Projectile(glm::vec2 pos, glm::vec2 size, Texture2D* sprite, float dmg, glm::vec3 color, float rotation, float speed, glm::vec2 velocity, float _lifetime, bool _indestructible)
{
	this->Position = pos;
	this->Size = size;
	this->Sprite = sprite;
	this->Color = color;
	this->Rotation = rotation;
	this->Speed = speed;
	this->Velocity = velocity;
	this->DamageDealt = dmg;
	this->lifetime = _lifetime;
	this->indestructible = _indestructible;
}

void Projectile::Update(float dt)
{
	this->Position += this->Velocity * dt * Speed;
	this->lifetime -= dt;
}
void Projectile::Hit()
{
	if(!indestructible)
		lifetime -= 1.0f;
}

bool Projectile::IsDead()
{
	return (this->lifetime <= 0.0f);
}