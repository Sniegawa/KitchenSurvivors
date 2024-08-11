#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "GameObject.h"

class Player : public GameObject
{
public:
	Player(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), float rotation = 0.0f);

	void ReduceCooldowns(float dt);
	void Shoot();

	unsigned int Level = 0;
	float LvlProgress = 0;
	int MaxHealth = 100;
	float Health = MaxHealth;
	float AttackSpeed = 1.0f;

	bool CanShoot = true;

private:
	unsigned int Kills;
	float AttackCooldown = 0.0f;
};

#endif // !PLAYER_HPP
