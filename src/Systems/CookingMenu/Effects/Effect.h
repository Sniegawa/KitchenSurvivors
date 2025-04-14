#pragma once

#include <iostream>
#include <string>
#include <memory>

class Player;
class Texture2D;
class Effect
{
public:
	Effect(int _id, std::string _name, std::string _desc, float _length, const Texture2D& _sprite, Player* _playerPointer) : ID(_id), Name(_name), Description(_desc), Length(_length), Sprite(_sprite), m_playerptr(_playerPointer) {};
	const int ID;
	const std::string Name;
	const std::string Description;
	const Texture2D& Sprite;

	float Length = 0;
	float Duration = 0;
	
	virtual void Start() 
	{
		this->Duration = Length;
	}
	
	virtual void Tick(float dt) 
	{
		this->Duration -= dt;
	}
	
	virtual void End() 
	{
		
	}

protected:

	Player* m_playerptr;
};
