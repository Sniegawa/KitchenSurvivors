#include "OrbitWeapon.h"

extern std::vector<std::shared_ptr<Projectile>> PlayerProjectiles;


OrbitWeapon::OrbitWeapon(std::string _sprite, std::string _name, PlayerStats* _stats, glm::vec2* _pos)
{
	this->sprite = _sprite;
	this->id = 1;
	this->name = _name;
	this->level = 1;
	this->p_Stats = _stats;
	this->p_PlayerPosition = _pos;
}

void OrbitWeapon::Shoot()
{

	Texture2D tex = ResourceManager::GetTexture(this->sprite);
	
	int k = 0;
	int n = this->p_Stats->projectileCount + 3;
	int r = 100;
	glm::vec2 playerpos = *p_PlayerPosition;
	for (; k < n; k++)
	{
		float tk = (2 * glm::pi<float>() * k) / n;
		glm::vec2 temppos = glm::vec2(cos(tk)*r, r*sin(tk)) + center + *p_PlayerPosition;
		PlayerProjectiles.push_back(std::make_unique<Projectile>(
			temppos,
			glm::vec2(tex.Width,tex.Height),
			tex,
			10,
			glm::vec3(1.0f),
			-atan2(temppos.x - playerpos.x,temppos.y - playerpos.y),
			0.0f,
			glm::vec2(0.0f),
			10
		));
	}
}

void OrbitWeapon::Update(float dt)
{
	return;
	this->pos += dt;
	int k = 0;
	int n = this->activeProjectiles.size();
	for (auto p : this->activeProjectiles)
	{
		float tk = 2 * glm::pi<float>() * k / n;
		p->Position =  (10.0f * glm::vec2(sin(this->pos), cos(this->pos))) + *p_PlayerPosition + center;
		k++;
	}
}