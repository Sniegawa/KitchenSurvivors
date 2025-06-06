#include "Projectile.h"

Projectile::Projectile(glm::vec2 pos, glm::vec2 size, Texture2D* sprite, Shader* shader, RenderLayer layer, float dmg, glm::vec3 color, float rotation, float speed, glm::vec2 velocity, float _lifetime, bool _indestructible)
	: GameObject(pos, size, rotation, sprite, shader, layer, color)
{
	this->SetRenderLayer(layer);
	this->Speed = speed;
	this->Velocity = velocity;
	this->DamageDealt = dmg;
	this->lifetime = _lifetime;
	this->indestructible = _indestructible;
}

void Projectile::Update(float dt)
{
	this->Move(this->Velocity * dt * Speed);
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