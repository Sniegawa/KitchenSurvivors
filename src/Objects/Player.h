#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "GameObject.h"
#include <vector>
#include "Projectile.h"
#include "../Weapons/Weapon.h"
#include "../Common.h"
#include <memory>

class Player : public GameObject
{
public:
	Player(glm::vec2 pos, glm::vec2 size, Texture2D sprite, std::vector<std::shared_ptr<Projectile>>* pprojptr, glm::vec3 color = glm::vec3(1.0f), float rotation = 0.0f);

	void TakeDamage(float amount);
	void UpdateCooldowns(float dt);
	PlayerStats stats;
	unsigned int Level = 0;
	float LvlProgress = 0;
	float Health = stats.MaxHealth;
	bool Alive = true;
	unsigned int Kills;
	
	Weapon *weapons[6];
private:

	float InvulnerabilityCD;
	std::vector<std::shared_ptr<Projectile>>* PlayerProjectilesPtr;
};

#endif // !PLAYER_HPP
