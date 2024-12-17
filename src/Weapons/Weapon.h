#pragma once
#include "string"
#include "../ResourceHandlers/Texture.h"
#include "../Objects/Player.h"
//#include "../Objects/Projectile.h"
#include "../Common.h"
#include <vector>
#include <memory>

enum UpgradeType
{
	WEAPON_UPGRADE_DAMAGE = 0,
	WEAPON_UPGRADE_PROJECTILES,
	WEAPON_UPGRADE_DURATION,
	WEAPON_UPGRADE_PENETRATION
};

struct Upgrade {
	UpgradeType Type;
	int Amount;
	Upgrade(UpgradeType _type, int _amount) : Type(_type), Amount(_amount) {};
	Upgrade() {};
};

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
	Player* p_Player;
	virtual void Shoot();
	virtual void LvlUp();
	virtual void Update(float dt);
	virtual ~Weapon();
	Weapon(std::string _sprite,std::string _name, PlayerStats* _stats, Player* _player,float _cooldown);
	Weapon();

protected:

	float cooldown;
	float remainingcd;
private:
	int BaseDamage;
};