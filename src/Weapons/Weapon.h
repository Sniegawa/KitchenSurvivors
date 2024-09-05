#pragma once
#include "string"
#include "../ResourceHandlers/Texture.h"
#include "../Objects/Projectile.h"
#include "../Common.h"
#include <vector>
#include <memory>
class Weapon
{
public:
	int id;
	std::string name;
	std::string sprite;
	int level;
	//enum? Evolution;
	PlayerStats* stats;
	std::vector<Projectile*> WeaponProjectiles;
	virtual void Shoot();
	virtual void LvlUp();

	virtual ~Weapon();
	Weapon(std::string _sprite,std::string _name, PlayerStats* _stats);
	Weapon();
private:
	//We need this to add projectiles that we shoot
	//std::vector<std::unique_ptr<Projectile>>* projectiles_ptr;
};