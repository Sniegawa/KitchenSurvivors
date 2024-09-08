#include "Weapon.h"

Weapon::~Weapon(){}

Weapon::Weapon() : id(0),name("Empty weapon slot"),p_PlayerPosition(nullptr),p_Stats(nullptr) { }

Weapon::Weapon(std::string _sprite, std::string _name, PlayerStats* _stats, glm::vec2* _pos, float _cooldown)
	: sprite(_sprite), name(_name),level(1), id(0), p_Stats(_stats), p_PlayerPosition(_pos), cooldown(_cooldown)
{
	this->Shoot();
}
void Weapon::Shoot() 
{

}

void Weapon::Update(float dt)
{
	this->remainingcd -= dt;
	if (this->remainingcd <= 0.0f)
	{
		//Without this-> to make use of abstraction
		Shoot();
		if(p_Stats != nullptr)
			this->remainingcd = this->cooldown / p_Stats->AttackSpeed;
	}
}

void Weapon::LvlUp()
{
	this->Shoot();
}
