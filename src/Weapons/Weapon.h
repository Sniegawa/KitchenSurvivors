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
	//enum? Evolution;
	int id;
	std::string name;
	std::string description;
	std::string sprite;
	int level;

	PlayerStats* p_Stats;
	glm::vec2* p_PlayerPosition;
	virtual void Shoot();
	virtual void LvlUp();
	virtual void Update(float dt);
	//virtual void HandleLvlUp();
	virtual ~Weapon();
	Weapon(std::string _sprite,std::string _name, PlayerStats* _stats, glm::vec2* _pos,float _cooldown);
	Weapon();

protected:
	const glm::vec2 center = glm::vec2(Common::ScreenSize.x / 2, Common::ScreenSize.y / 2);

	float cooldown;
	float remainingcd;
private:
	int BaseDamage;
};