#include "Projectile.h"

void Projectile::UpdatePosition(float dt)
{
	this->Position += this->Velocity * dt * Speed;
}