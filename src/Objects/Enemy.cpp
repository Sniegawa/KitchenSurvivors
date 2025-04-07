#include "Enemy.h"

Enemy::Enemy(glm::vec2 pos, glm::vec2 size, Texture2D* sprite, Shader* shader, RenderLayer layer,Player* _player, float health, glm::vec3 color, float rotation) 
	: GameObject(pos,size,rotation,sprite,shader,layer,color)
{
	this->SetRenderLayer(layer);
	this->m_Health = health;
	this->m_isDead = false;
	this->m_HurtTiming = 0.0f;
	this->p_player = _player;
}

void Enemy::TakeDamage(float amount)
{
	//Change so it can't take damage from same source but it can from multiple
	if (this->m_HurtTiming > 0.0f)
		return;
	this->m_Health -= amount;
	if (this->m_Health <= 0.0f)
	{
		this->m_isDead = true;
		return;
	}
	this->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
	this->m_HurtTiming = 0.3f;
}

void Enemy::Update(float dt)
{
	this->m_HurtTiming -= dt;
	if (this->m_HurtTiming <= 0.0f)
	{
		this->SetColor(glm::vec3(1.0f));
	}
	
	glm::vec2 DirectionToPlayer = glm::normalize(this->GetPosition() - p_player->GetPosition() - glm::vec2(p_player->GetSprite()->Width, p_player->GetSprite()->Height) * 0.5f);

	this->m_Position -= DirectionToPlayer * 25.0f * dt;

}