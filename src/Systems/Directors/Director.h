#pragma once

#include <vector>
#include <memory>
#include "Objects/Enemy.h"
#include <glm.hpp>

struct SpawnCard
{
	Enemy* EnemyToSpawn;
	int Weight;
	int Cost;
};

struct EnemySpawnPoint
{
	glm::vec2 pos;

	bool isValid()
};

class Director
{
public:
	Director(unsigned int* _seed);

	void Update(float dt);

	void Spawn();
private:
	int m_credits;

	std::vector<SpawnCard> m_SpawnCards;
	std::vector<std::shared_ptr<Enemy>>* m_enemyVector_ptr;

	unsigned int* m_seed;

	float m_SpawnCooldown = 10;
	float m_CurrentCooldown;


};