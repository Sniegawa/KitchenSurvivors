#pragma once
#include "Effect.h"


class E_DoubleTrouble : public Effect
{
public:

	E_DoubleTrouble(std::string _name, std::string _desc, const Texture2D& _spriteID, Player* _playerPointer) : Effect(_name, _desc, _spriteID, _playerPointer) {};

	void Start() override;
	void Tick(float dt) override;
	void End() override;
};