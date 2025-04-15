#pragma once

#include "GameObject.h"
#include "Player.h"

class Enemy : public GameObject 
{
public:
	Enemy(glm::vec2 pos, glm::vec2 size, Texture2D* sprite, Shader* shader, RenderLayer layer, float health = 25.0f, glm::vec3 color = glm::vec3(1.0f), float rotation = 0.0f);
	void TakeDamage(float amount);
	void Update(float dt);
	void NavigateToPlayer(Player* player_ptr, float dt);
	const float& GetHealth() const { return this->m_Health; }
	const bool& isDead() const { return this->m_isDead; }

private:
	float m_Health;
	float m_MaxHealth;
	float m_HurtTiming;
	bool m_isDead;

};