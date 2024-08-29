#pragma once
#include "string"
#include "Texture.h"
#include "Projectile.h"
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
private:
	Texture2D sprite;
	
};