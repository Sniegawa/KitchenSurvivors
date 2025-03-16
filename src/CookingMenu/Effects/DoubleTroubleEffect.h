#pragma once
#include "Effect.h"

class E_DoubleTroube : Effect
{
public:

	E_DoubleTroube(std::string _name, std::string _desc, const Texture2D& _spriteID, std::shared_ptr<Player> _playerPointer) : Effect(_name, _desc, _spriteID, _playerPointer) {};

	void Start() override 
	{
		this->m_playerptr->stats.AttackSpeed += 1.0f;
	};
	void Tick(float dt) override
	{
		this->Duration -= dt;
		if (this->Duration <= 0.0f)
		{
			End();
		}
	};
	void End() override
	{
		this->m_playerptr->stats.AttackSpeed -= 1.0f;
	};
};