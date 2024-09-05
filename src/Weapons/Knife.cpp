#include "Knife.h"

float MousePlayerAngle;

glm::vec2 PlayerPosition;

std::vector<std::unique_ptr<Projectile>> PlayerProjectiles;

glm::vec2 ScreenCenter;

void KnifeWeapon::Shoot()
{
	Texture2D knifetex = ResourceManager::GetTexture(this->sprite);

	int projectileCount = this->stats->projectileCount;

	if (projectileCount > 1) {

		if (projectileCount % 2 == 0 && projectileCount < 25)
		{
			for (int i = -projectileCount / 2; i < projectileCount / 2 + 1; i++)
			{
				if (i == 0)
					continue;
				float angleoffset = (glm::pi<float>() / 12) * i;
				PlayerProjectiles.push_back(
					std::make_unique<Projectile>(Projectile(
						PlayerPosition + ScreenCenter,
						glm::vec2(knifetex.Width, knifetex.Height),
						knifetex,
						25.0f,
						glm::vec3(1.0f),
						MousePlayerAngle + glm::pi<float>() / 2.0f + angleoffset,
						500,
						glm::vec2(-sin(MousePlayerAngle + angleoffset), cos(MousePlayerAngle + angleoffset))
					))
				);
			}
		}
		else
		{
			for (int i = -projectileCount / 2; i < projectileCount / 2 + 1; i++)
			{
				float angleoffset = (glm::pi<float>() / 12) * i;
				PlayerProjectiles.push_back(
					std::make_unique<Projectile>(Projectile(
						PlayerPosition + ScreenCenter,
						glm::vec2(knifetex.Width, knifetex.Height),
						knifetex,
						25.0f,
						glm::vec3(1.0f),
						MousePlayerAngle + glm::pi<float>() / 2.0f + angleoffset,
						500,
						glm::vec2(-sin(MousePlayerAngle + angleoffset), cos(MousePlayerAngle + angleoffset))
					))
				);
			}
		}
	}
	else
	{
		PlayerProjectiles.push_back(
			std::make_unique<Projectile>(Projectile(
				PlayerPosition + ScreenCenter,
				glm::vec2(knifetex.Width, knifetex.Height),
				knifetex,
				25.0f,
				glm::vec3(1.0f),
				MousePlayerAngle + glm::pi<float>() / 2.0f,
				500,
				glm::vec2(-sin(MousePlayerAngle), cos(MousePlayerAngle))
			))
		);
	}
}

KnifeWeapon::KnifeWeapon(std::string _sprite, std::string _name, PlayerStats* _stats)
{
	this->sprite = _sprite;
	this->id = 1;
	this->name = _name;
	this->level = 1;
	this->stats = _stats;
}