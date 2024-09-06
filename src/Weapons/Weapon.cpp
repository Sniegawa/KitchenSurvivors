#include "Weapon.h"

Weapon::~Weapon(){}

Weapon::Weapon() : id(0),name("Empty weapon slot"),p_PlayerPosition(nullptr),p_Stats(nullptr) { }

Weapon::Weapon(std::string _sprite, std::string _name, PlayerStats* _stats,glm::vec2* _pos)
	: sprite(_sprite), name(_name),level(1), id(0), p_Stats(_stats), p_PlayerPosition(_pos)
{

}
void Weapon::Shoot() 
{

}

void Weapon::Update(float dt)
{

}

void Weapon::LvlUp()
{
	this->Shoot();
}