#include "ThrownWeapon.h"

float MousePlayerAngle;

std::vector<std::shared_ptr<Projectile>> PlayerProjectiles;

void CreateProjectile(std::string sprite,glm::vec2 *player_pos, glm::vec2 center,float angleoffset = 0.0f)
{
	Texture2D knifetex = ResourceManager::GetTexture(sprite);
	PlayerProjectiles.push_back(
		std::make_shared<Projectile>(Projectile(
			*player_pos + center,
			glm::vec2(knifetex.Width, knifetex.Height),
			&ResourceManager::GetTexture(sprite),
			25.0f,
			glm::vec3(1.0f),
			MousePlayerAngle + angleoffset + glm::pi<float>(),
			500,
			glm::vec2(-sin(MousePlayerAngle + angleoffset), cos(MousePlayerAngle + angleoffset))
		))
	);
}

void ThrownWeapon::Shoot()
{

	
	int projectileCount = this->p_Stats->projectileCount + this->AdditionalProjectiles;

	if (projectileCount > 1) {

		if (projectileCount % 2 == 0 /*&& projectileCount < 25*/)
		{
			for (int i = -projectileCount / 2; i < projectileCount / 2 + 1; i++)
			{
				if (i == 0)
					continue;

				float angleoffset = (glm::pi<float>() / 12) * i;
				CreateProjectile(this->sprite, p_PlayerPosition, center, angleoffset);
			}
		}
		else
		{
			for (int i = -projectileCount / 2; i < projectileCount / 2 + 1; i++)
			{
				float angleoffset = (glm::pi<float>() / 12) * i;
				CreateProjectile(this->sprite, p_PlayerPosition, center, angleoffset);
			}
		}
	}
	else
	{
		CreateProjectile(this->sprite, p_PlayerPosition, center);
	}
}

ThrownWeapon::ThrownWeapon(std::string _sprite, std::string _name, std::string _desc, PlayerStats* _stats, glm::vec2* _pos,float _cooldown)
{
	this->sprite = _sprite;
	this->id = 1;
	this->name = _name;
	this->level = 1;
	this->p_Stats = _stats;
	this->p_PlayerPosition = _pos;
	this->cooldown = _cooldown;
	this->description = _desc;
	lvlupScheme.insert(std::pair<int, Upgrade>(2, Upgrade(WEAPON_UPGRADE_PROJECTILES, 1)));
	lvlupScheme.insert(std::pair<int, Upgrade>(3, Upgrade(WEAPON_UPGRADE_PROJECTILES, 1)));
	lvlupScheme.insert(std::pair<int, Upgrade>(4, Upgrade(WEAPON_UPGRADE_PROJECTILES, 1)));
}

void ThrownWeapon::LvlUp()
{
	Weapon::LvlUp();
	Upgrade up = this->lvlupScheme[this->level];

	switch (up.Type)
	{
	case WEAPON_UPGRADE_DAMAGE:
	case WEAPON_UPGRADE_DURATION:
	case WEAPON_UPGRADE_PENETRATION:
		break;
	case WEAPON_UPGRADE_PROJECTILES:
		this->AdditionalProjectiles += up.Amount;
	default:
		break;
	}
}