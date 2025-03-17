#pragma once
#include "Effect.h"


class E_DoubleTrouble : public Effect
{
public:

	E_DoubleTrouble(int _id,std::string _name, std::string _desc, const Texture2D& _spriteID, Player* _playerPointer) : Effect(_id,_name, _desc, _spriteID, _playerPointer) {};

	void Start(float length) override;
	void Tick(float dt) override;
	void End() override;
};