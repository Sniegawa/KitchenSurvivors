#include "DoubleTroubleEffect.h"

#include "../../Objects/Player.h"

void E_DoubleTrouble::Tick(float dt)
{
	this->Duration -= dt;
	if (this->Duration <= 0.0f)
	{
		End();
	}
}

void E_DoubleTrouble::Start()
{
	//this->m_playerptr->stats.AttackSpeed += 1.0f;
}

void E_DoubleTrouble::End()
{
	//this->m_playerptr->stats.AttackSpeed -= 1.0f;
}