#pragma once
#include "Weapon.h"

class OrbitWeapon : public Weapon
{
public:
	OrbitWeapon(std::string _sprite, std::string _name, PlayerStats* _stats, glm::vec2* _pos);
	void Shoot() override;
	void Update(float dt) override;
private:
	std::vector<std::vector<std::shared_ptr<Projectile>>> activeProjectiles;
	float pos;
};