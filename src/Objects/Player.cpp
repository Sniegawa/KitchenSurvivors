#include "Player.h"

Player::Player(glm::vec2 pos, glm::vec2 size, Texture2D* sprite, Shader* shader, RenderLayer layer, std::vector<std::shared_ptr<Projectile>>* pprojptr, glm::vec3 color, float rotation)
	: GameObject(pos, size, rotation, sprite, shader, layer, color)
{
	this->SetRenderLayer(layer);
	this->Level = 1;
	this->InvulnerabilityCD = 0.0f;
	this->stats.projectileCount = 1;
	this->PlayerProjectilesPtr = pprojptr;
	this->xp = 0;
	this->xpToLvl = Common::lvlmap[this->Level];
	this->NormalMap = &ResourceManager::GetTexture("pizza_n"); // TEMP

}


void Player::TakeDamage(float amount)
{	
	if (this->InvulnerabilityCD > 0.0f)
		return;
	this->Health -= amount;
	if (this->Health <= 0.0f)
	{
		this->Alive = false;
	}
	this->InvulnerabilityCD = 0.1f; // Shouldn't be fixed amount
}

void Player::UpdateCooldowns(float dt)
{
	this->InvulnerabilityCD -= dt;
	
	//Update effects
	for (int i = 0; i < this->effects.size(); i++)
	{
		Effect* eff = this->effects[i];
		eff->Tick(dt);

		//Handle effect expiration
		if (eff->Duration <= 0.0f)
		{
			eff->End();
			this->effects.erase(this->effects.begin() + i);
		}

	}
}

void Player::GetXp(int type)
{
	return;// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	/*
	xp += type * this->stats.ExpirienceMultiplier;

	if (this->xp >= this->xpToLvl)
	{
		this->Level++;
		this->xp -= this->xpToLvl;
		
		if (this->Level > Common::lvlmap.size())
			this->xpToLvl = Common::lvlmap[Common::lvlmap.size()];
		else
			this->xpToLvl = Common::lvlmap[this->Level];
	}
	*/
}

void Player::AddEffect(Effect* effect, float Length)
{
	for (int i = 0; i < this->effects.size(); i++)
	{
		if (this->effects[i]->ID == effect->ID)
		{
			this->effects[i]->End();
			this->effects[i]->Start();
			return;
		}
	}
	this->effects.push_back(effect);
	effect->Start();
}