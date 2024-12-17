#include "Weapon.h"
Weapon::~Weapon(){}

Weapon::Weapon() : id(0),name("Empty weapon slot"), p_Player(nullptr),p_Stats(nullptr) { }

Weapon::Weapon(std::string _sprite, std::string _name, PlayerStats* _stats, Player* _player, float _cooldown)
	: sprite(_sprite), name(_name),level(1), id(0), p_Stats(_stats), p_Player(_player), cooldown(_cooldown)
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
	Shoot();
	this->level++;
}
