#pragma once
#include "Weapon.h"

class KnifeWeapon : public Weapon
{
public:
	KnifeWeapon(std::string _sprite, std::string _name, PlayerStats* _stats, glm::vec2* _pos, float _cooldown);
	void Shoot() override;
};