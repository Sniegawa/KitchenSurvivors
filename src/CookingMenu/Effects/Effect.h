#pragma once

#include <iostream>
#include <string>
#include <memory>

class Player;
class Texture2D;
class Effect
{
public:
	Effect(std::string _name, std::string _desc, const Texture2D& _sprite, Player* _playerPointer);

	const std::string Name;
	const std::string Description;
	const Texture2D& Sprite;

	virtual void Start() { std::cout << "You have called base effect Start function"<<std::endl; }
	virtual void Tick(float dt) { std::cout << "You have called base effect Tick function" << std::endl; }
	virtual void End() { std::cout << "You have called base effect End function" << std::endl; }
protected:
	Player* m_playerptr;
	float Duration = 0;
};
