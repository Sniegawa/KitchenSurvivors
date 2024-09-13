#pragma once
#include "Weapon.h"

class KnifeWeapon : public Weapon
{
public:
	KnifeWeapon(std::string _sprite, std::string _name, std::string _desc, PlayerStats* _stats, glm::vec2* _pos, float _cooldown);
	void Shoot() override;
};