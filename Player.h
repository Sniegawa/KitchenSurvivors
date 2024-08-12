#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "GameObject.h"

struct PlayerStats {
	int projectileCount = 1;
};

class Player : public GameObject
{
public:
	Player(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), float rotation = 0.0f);

	void TakeDamage(float amount);

	unsigned int Level = 0;
	float LvlProgress = 0;
	int MaxHealth = 100;
	float Health = MaxHealth;
	float AttackSpeed = 1.50f;
	bool Alive = true;

	PlayerStats stats;

private:
	unsigned int Kills;
};

#endif // !PLAYER_HPP
