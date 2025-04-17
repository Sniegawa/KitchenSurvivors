#pragma once

#include <vector>
#include <memory>
#include "Objects/Enemy.h"
#include "DifficultyManager.h"
#include <glm.hpp>
#include <functional>

struct EnemyInformation
{
	glm::vec2 size;
	std::string sprite;
	std::string shader;
	float Health;
};

struct SpawnCard
{
	int Weight;
	int Cost;
	std::function<std::shared_ptr<Enemy>(const glm::vec2&,const EnemyInformation& info)> createEnemy;
	EnemyInformation info;
};

struct EnemySpawnPoint
{
	glm::vec2 pos;

	bool isValid(glm::vec2& PlayerPosition) 
	{ 
		return glm::distance(this->pos, PlayerPosition) <= 200.0f; 
	}
};

class Director
{
public:
	Director(unsigned int* _seed, DifficultyManager** _diffManager, std::vector<std::shared_ptr<Enemy>>* enemies_ptr);

	void Update(float dt);
	
	//Should be private but for debug purposes i leave it public
	float m_credits;
private:
	
	bool Spawn();


	std::vector<SpawnCard> m_SpawnCards;
	std::vector<std::shared_ptr<Enemy>>* m_enemyVector_ptr;

	unsigned int* m_seed;

	//double pointer, need refactoring asap
	DifficultyManager** m_diffManager;
	//Should be based on difficulty
	float m_SpawnCooldown = 30.0f;
	float m_CurrentCooldown = 15.0f;
};