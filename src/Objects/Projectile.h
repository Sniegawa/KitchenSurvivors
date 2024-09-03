#pragma once
#include "GameObject.h"

class Projectile : public GameObject
{
public:
	Projectile(glm::vec2 pos, glm::vec2 size, Texture2D sprite, float dmg, glm::vec3 color = glm::vec3(1.0f), float rotation = 0.0f,float speed = 10.0f,glm::vec2 velocity = glm::vec2(0.0f));
	
	glm::vec2 Velocity = glm::vec2(0.0f);
	float Speed;
	float DamageDealt;
	void Update(float dt);
	void Hit();
	bool IsDead();
private:
	float lifetime;
};