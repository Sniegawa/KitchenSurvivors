#include "Player.h"

Player::Player(glm::vec2 pos, glm::vec2 size, Texture2D sprite, std::vector<std::unique_ptr<Projectile>>* pprojptr, glm::vec3 color, float rotation)
{
	this->Position = pos;
	this->Size = size;
	this->Sprite = sprite;
	this->Color = color;
	this->Rotation = rotation;
	this->Kills = 0;
	this->Level = 1;
	this->stats.projectileCount = 1;
	this->PlayerProjectilesPtr = pprojptr;
}


void Player::TakeDamage(float amount)
{	
	if (this->InvulnerabilityCD > 0.0f)
		return;
	this->Health -= amount;
	if (this->Health <= 0.0f)
	{
		this -> Alive = false;
	}
	this->InvulnerabilityCD = 0.1f;
}

void Player::UpdateCooldowns(float dt)
{
	this->InvulnerabilityCD -= dt;
}