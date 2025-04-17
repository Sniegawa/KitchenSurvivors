#include "Enemy.h"

Enemy::Enemy(glm::vec2 pos, glm::vec2 size, Texture2D* sprite, Shader* shader, RenderLayer layer, float health, glm::vec3 color, float rotation) 
	: GameObject(pos,size,rotation,sprite,shader,layer,color)
{
	m_renderLayer = layer;
	m_Health = health;
	m_isDead = false;
	m_HurtTiming = 0.0f;
}

void Enemy::TakeDamage(float amount)
{
	//Change so it can't take damage from same source but it can from multiple
	if (m_HurtTiming > 0.0f)
		return;
	m_Health -= amount;
	if (m_Health <= 0.0f)
	{
		m_isDead = true;
		return;
	}
	this->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
	m_HurtTiming = 0.3f;
}

void Enemy::Update(float dt)
{
	m_HurtTiming -= dt;
}

void Enemy::NavigateToPlayer(Player* player_ptr, float dt)
{
	glm::vec2 DirectionToPlayer = glm::normalize(this->GetPosition() - player_ptr->GetPosition() - glm::vec2(player_ptr->GetSprite()->Width, player_ptr->GetSprite()->Height) * 0.5f);
	m_Position -= DirectionToPlayer * 25.0f * dt;
	m_Moved = true;
}