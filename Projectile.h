#pragma once
#include "GameObject.h"

class Projectile : public GameObject
{
public:

	Projectile(const GameObject& g, glm::vec2 velocity, float speed = 16.0f) : GameObject(g), Velocity(velocity), Speed(speed){}

	glm::vec2 Velocity;
	float Speed;
	void UpdatePosition(float dt);
};