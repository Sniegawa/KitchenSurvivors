#include "DoubleTroubleEffect.h"

#include "../../Objects/Player.h"

void E_DoubleTrouble::Tick(float dt)
{
	Effect::Tick(dt);
}

void E_DoubleTrouble::Start(float length)
{
	Effect::Start(length);
	this->m_playerptr->stats.AttackSpeed += 1.0f;
}

void E_DoubleTrouble::End()
{
	this->m_playerptr->stats.AttackSpeed -= 1.0f;
}