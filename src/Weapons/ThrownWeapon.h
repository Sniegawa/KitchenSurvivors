#pragma once
#include "Weapon.h"

class ThrownWeapon : public Weapon
{
public:
	ThrownWeapon(std::string _sprite, std::string _name, std::string _desc, PlayerStats* _stats,Player* _player, float _cooldown);
	void Shoot() override;
	void LvlUp() override;
private:
	std::map<int, Upgrade> lvlupScheme;
	int AdditionalProjectiles;
};
