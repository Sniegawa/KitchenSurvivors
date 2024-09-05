#pragma once
#include "Weapon.h"

class KnifeWeapon : public Weapon
{
public:
	KnifeWeapon(std::string _sprite, std::string _name, PlayerStats* _stats);
	void Shoot() override;
};