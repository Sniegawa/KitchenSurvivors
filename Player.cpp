#include "Player.h"

Player::Player(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, float rotation)
{
	this->Position = pos;
	this->Size = size;
	this->Sprite = sprite;
	this->Color = color;
	this->Rotation = rotation;
	this->Kills = 0;
	this->Level = 1;
}

void Player::ReduceCooldowns(float dt)
{
	AttackCooldown -= dt;

	if (AttackCooldown <= 0.0f)
	{
		CanShoot = true;
	}
	else 
	{
		CanShoot = false;
	}
}

void Player::Shoot()
{
	CanShoot = false;
	AttackCooldown = 1.0f / AttackSpeed;
}