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
	this->stats.projectileCount = 1;
}


void Player::TakeDamage(float amount)
{
	this->Health -= amount;
	printf("Player hit! current healt %f", this->Health);
	if (this->Health <= 0.0f)
	{
		this -> Alive = false;
	}
}