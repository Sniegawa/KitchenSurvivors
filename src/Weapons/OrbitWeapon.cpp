#include "OrbitWeapon.h"
extern std::vector<std::shared_ptr<Projectile>> PlayerProjectiles;


OrbitWeapon::OrbitWeapon(std::string _sprite, std::string _name, PlayerStats* _stats, Player* _player,float _cooldown)
{
	this->sprite = _sprite;
	this->id = 1;
	this->name = _name;
	this->level = 1;
	this->p_Stats = _stats;
	this->p_Player = _player;
	this->cooldown = _cooldown;
}

void OrbitWeapon::Shoot()
{	
	if (this->activeProjectiles.size() > 0)
		return;

	Texture2D tex = ResourceManager::GetTexture(this->sprite);
	
	std::vector<std::shared_ptr<Projectile>> batch;

	int k = 0;
	int n = this->p_Stats->projectileCount + this->AdditionalProjectiles;
	int r = 100;
	glm::vec2 playerpos = p_Player->Position + p_Player->Size * 0.5f;
	for (; k < n; k++)
	{
		float tk = (2 * glm::pi<float>() * k) / n;
		glm::vec2 temppos = glm::vec2(cos(tk)*r, r*sin(tk)) + playerpos;
		
		float rotation = -atan2(cos(tk), sin(tk)) + glm::pi<float>()/2;
		
		float Damage = 10.0f;

		PlayerProjectiles.push_back(std::make_unique<Projectile>(
			temppos,
			glm::vec2(tex.Width,tex.Height),
			&ResourceManager::GetTexture(this->sprite),
			ResourceManager::GetShaderPtr("instancedSprite"),
			PROJECTILES,
			Damage,
			glm::vec3(1.0f),
			rotation,
			0.0f,
			glm::vec2(0.0f),
			10,
			true
		));
		batch.push_back(PlayerProjectiles.back());
	}

	activeProjectiles.push_back(batch);
}

void OrbitWeapon::Update(float dt)
{	
	glm::vec2 playerpos = p_Player->Position + p_Player->Size * 0.5f;// -glm::vec2(8, 8);
	//Calling base class Update method
	Weapon::Update(dt);

	for (int i = 0; i < this->activeProjectiles.size();i++)
	{
		std::vector<std::shared_ptr<Projectile>> pl = this->activeProjectiles[i];
		int n = pl.size();
		if (pl[0]->IsDead())
		{
			this->activeProjectiles.clear();
			return;
		}

		int k = 0;
		int r = 100;
		
		for (auto p : pl)
		{
			float lt = p->lifetime;
			float tk = 2 * glm::pi<float>() * k / n;
			float rotation = -atan2(cos(tk-lt), sin(tk-lt)) + glm::pi<float>() / 2;
			p->Position = glm::vec2(cos(tk - lt) * r, r * sin(tk - lt)) + playerpos;
			p->Rotation = rotation;
			k++;
		}
	}
}