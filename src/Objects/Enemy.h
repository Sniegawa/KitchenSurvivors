#pragma once

#include "GameObject.h"

class Enemy : public GameObject 
{
public:
	Enemy(glm::vec2 pos, glm::vec2 size, Texture2D sprite, float health = 25.0f, glm::vec3 color = glm::vec3(1.0f), float rotation = 0.0f);
	void TakeDamage(float amount);
	void Update(float dt);
	bool isDead;
	float Health;
private:
	float HurtTiming;

};