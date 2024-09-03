#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "GameObject.h"
#include <vector>
#include "Projectile.h"
#include "../Weapons/Weapon.h"
#include <memory>
struct PlayerStats {
	float AttackSpeed = 1.0f;
	int MaxHealth = 100;
	int projectileCount = 1;
	float PlayerSpeed = 75.0f;
};

class Player : public GameObject
{
public:
	Player(glm::vec2 pos, glm::vec2 size, Texture2D sprite, std::vector<std::unique_ptr<Projectile>>* pprojptr, glm::vec3 color = glm::vec3(1.0f), float rotation = 0.0f);

	void TakeDamage(float amount);
	void UpdateCooldowns(float dt);
	PlayerStats stats;
	unsigned int Level = 0;
	float LvlProgress = 0;
	float Health = stats.MaxHealth;
	bool Alive = true;
	unsigned int Kills;
	
private:
	Weapon weapons[6];

	float InvulnerabilityCD;
	std::vector<std::unique_ptr<Projectile>>* PlayerProjectilesPtr;
};

#endif // !PLAYER_HPP
