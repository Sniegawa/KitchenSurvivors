#pragma once
#include "Weapon.h"

class ThrownWeapon : public Weapon
{
public:
	ThrownWeapon(std::string _sprite, std::string _name, std::string _desc, PlayerStats* _stats, glm::vec2* _pos, float _cooldown);
	void Shoot() override;
	void LvlUp() override;
};