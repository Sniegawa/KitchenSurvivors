#include "Enemy.h"

Enemy::Enemy(glm::vec2 pos, glm::vec2 size, Texture2D sprite, float health, glm::vec3 color, float rotation)
{
	this->Position = pos;
	this->Size = size;
	this->Sprite = sprite;
	this->Color = color;
	this->Rotation = rotation;
	this->Health = health;
	this->isDead = false;
	this->HurtTiming = 0.0f;
}

void Enemy::TakeDamage(float amount)
{
	if (this->HurtTiming > 0.0f)
		return;
	this->Health -= amount;
	if (this->Health <= 0.0f)
	{
		this->isDead = true;
		return;
	}
	this->Color = glm::vec3(1.0f, 0.0f, 0.0f);
	this->HurtTiming = 0.3f;
}

void Enemy::Update(float dt)
{
	this->HurtTiming -= dt;
	if (this->HurtTiming <= 0.0f)
	{
		this->Color = glm::vec3(1.0f);
	}
}