#pragma once

#include <iostream>
#include <string>
#include <memory>

class Effect
{
public:
	Effect(std::string _name,std::string _desc,const Texture2D& _spriteID,std::shared_ptr<Player> _playerPointer) : Name(_name),Description(_desc),SpriteID(_spriteID), m_playerptr(_playerPointer) {}

	const std::string Name;
	const std::string Description;
	const Texture2D& SpriteID;

	virtual void Start() { std::cout << "You have called base effect Start function"<<std::endl; }
	virtual void Tick(float dt) { std::cout << "You have called base effect Tick function" << std::endl; }
	virtual void End() { std::cout << "You have called base effect End function" << std::endl; }
protected:
	std::shared_ptr<Player> m_playerptr;
	float Duration;
};
