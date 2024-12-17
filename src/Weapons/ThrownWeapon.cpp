#include "ThrownWeapon.h"

std::vector<std::shared_ptr<Projectile>> PlayerProjectiles;

void CreateProjectile(std::string sprite,Player* p_player,float angleoffset = 0.0f)
{
	Texture2D knifetex = ResourceManager::GetTexture(sprite);
	PlayerProjectiles.push_back(
		std::make_shared<Projectile>(Projectile(
			p_player->Position + p_player->Size * 0.5f ,
			glm::vec2(knifetex.Width, knifetex.Height),
			&ResourceManager::GetTexture(sprite),
			ResourceManager::GetShaderPtr("instancedSprite"),
			PROJECTILES,
			25.0f,
			glm::vec3(1.0f),
			Common::MousePlayerAngle + angleoffset + glm::pi<float>(),
			500,
			glm::vec2(-sin(Common::MousePlayerAngle + angleoffset), cos(Common::MousePlayerAngle + angleoffset))
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
				CreateProjectile(this->sprite, p_Player, angleoffset);
			}
		}
		else
		{
			for (int i = -projectileCount / 2; i < projectileCount / 2 + 1; i++)
			{
				float angleoffset = (glm::pi<float>() / 12) * i;
				CreateProjectile(this->sprite, p_Player, angleoffset);
			}
		}
	}
	else
	{
		CreateProjectile(this->sprite, p_Player);
	}
}

ThrownWeapon::ThrownWeapon(std::string _sprite, std::string _name, std::string _desc, PlayerStats* _stats, Player* _player,float _cooldown)
{
	this->sprite = _sprite;
	this->id = 1;
	this->name = _name;
	this->level = 1;
	this->p_Stats = _stats;
	this->p_Player = _player;
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