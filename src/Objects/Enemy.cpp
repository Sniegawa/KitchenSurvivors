#include "Enemy.h"

Enemy::Enemy(glm::vec2 pos, glm::vec2 size, Texture2D* sprite, Shader* shader, RenderLayer layer,Player* _player, float health, glm::vec3 color, float rotation)
{
	this->Position = pos;
	this->Size = size;
	this->Sprite = sprite;
	this->SetRenderLayer(layer);
	this->shader = shader;
	this->Color = color;
	this->Rotation = rotation;
	this->Health = health;
	this->isDead = false;
	this->HurtTiming = 0.0f;
	this->p_player = _player;
}

void Enemy::TakeDamage(float amount)
{
	//Change so it can't take damage from same source but it can from multiple
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

	glm::vec2 DirectionToPlayer = glm::normalize(this->Position - p_player->Position - glm::vec2(p_player->Sprite->Width, p_player->Sprite->Height)*0.5f);

	this->Position -= DirectionToPlayer * 25.0f * dt;

}