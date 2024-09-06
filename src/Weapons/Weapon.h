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
	PlayerStats* p_Stats;
	glm::vec2* p_PlayerPosition;
	virtual void Shoot();
	virtual void LvlUp();
	virtual void Update(float dt);
	virtual ~Weapon();
	Weapon(std::string _sprite,std::string _name, PlayerStats* _stats, glm::vec2* _pos);
	Weapon();
private:
	std::vector<Projectile*> p_WeaponProjectiles;

protected:
	const glm::vec2 center = glm::vec2(ScreenSize.x / 2, ScreenSize.y / 2);

	//We need this to add projectiles that we shoot
	//std::vector<std::unique_ptr<Projectile>>* projectiles_ptr;
};