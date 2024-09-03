#pragma once
#include "string"
#include "../ResourceHandlers/Texture.h"
#include "../Objects/Projectile.h"
#include <vector>
class Weapon
{
public:
	std::string name;
	int level;
	//enum? Evolution;
	std::vector<Projectile*> WeaponProjectiles;
	virtual void shoot();
	virtual ~Weapon();
	Weapon(Texture2D _sprite,std::string _name);
	Weapon();
private:
	Texture2D sprite;
	
};