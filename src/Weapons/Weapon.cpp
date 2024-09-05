#include "Weapon.h"

Weapon::~Weapon(){}

Weapon::Weapon() : id(0),name("Empty weapon slot") { }

Weapon::Weapon(std::string _sprite, std::string _name, PlayerStats* _stats) 
	: sprite(_sprite), name(_name),level(1), id(0), stats(_stats)
{

}
void Weapon::Shoot() 
{

}
void Weapon::LvlUp()
{
	this->Shoot();
}