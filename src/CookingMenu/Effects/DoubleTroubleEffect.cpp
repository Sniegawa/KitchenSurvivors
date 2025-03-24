#include "DoubleTroubleEffect.h"

#include "../../Objects/Player.h"

void E_DoubleTrouble::Tick(float dt)
{
	Effect::Tick(dt);
}

void E_DoubleTrouble::Start()
{
	Effect::Start();
	this->m_playerptr->stats.AttackSpeed += 1.0f;
	this->m_playerptr->Color = glm::vec3(1.0f, 0.0f, 0.0f);
}

void E_DoubleTrouble::End()
{
	this->m_playerptr->stats.AttackSpeed -= 1.0f;
	this->m_playerptr->Color = glm::vec3(1.0f, 1.0f, 1.0f);
}