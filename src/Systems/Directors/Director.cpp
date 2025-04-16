#include "Director.h"
#include "Objects/Enemy.h"

#include <algorithm>
#include <random>
#include <functional>

//#include "Objects/Enemies/EnemyIncludes.h"

//Template function that can be stored as lambda wich determines it's type
//Used for instancing different types of enemies
template <typename T>
std::function<std::shared_ptr<Enemy>(const glm::vec2&, const EnemyInformation& info)> createEnemyFactory()
{
	return [](const glm::vec2& pos, const EnemyInformation& info) -> std::shared_ptr<Enemy>
		{
			return std::make_shared<T>(
				pos,
				info.size,
				&ResourceManager::GetTexture(info.sprite),
				ResourceManager::GetShaderPtr(info.shader),
				RenderLayer::ENEMY,
				info.Health
				);
		};
}

Director::Director(unsigned int* _seed, DifficultyManager** _diffManager, std::vector<std::shared_ptr<Enemy>>* enemies_ptr)
{
	m_enemyVector_ptr = enemies_ptr;
	m_diffManager = _diffManager;
	m_seed = _seed;
	m_CurrentCooldown = m_SpawnCooldown;
	m_credits = 0;

	m_SpawnCards =
	{
		{10,25,createEnemyFactory<Enemy>(),{glm::vec2(25.0f),"pizza","instancedSprite",25}},
		{10,50,createEnemyFactory<Enemy>(),{glm::vec2(50.0f),"tomato","instancedSprite",100}}
	};

}

bool Director::Spawn()
{
	std::cout << "trying to spawn : " << m_credits << std::endl;
	std::vector<SpawnCard*> AffordableCards;
	int maxWeight = 0;

	for (int i = 0; i < m_SpawnCards.size(); ++i)
	{
		SpawnCard* card = &m_SpawnCards[i];
		if (card->Cost < m_credits)
		{
			AffordableCards.emplace_back(card);
			maxWeight += card->Weight;
		}
	}

	if (AffordableCards.size() <= 0) return false;

	//Shuffle affordable cards for random spawning
	auto rng = std::default_random_engine{*m_seed};
	std::shuffle(std::begin(AffordableCards), std::end(AffordableCards), rng);

	int randomWeight = rand() % maxWeight;

	SpawnCard* cardToSpawn = nullptr;

	for (int i = 0; i < AffordableCards.size(); ++i)
	{
		randomWeight -= AffordableCards[i]->Weight;
		if (randomWeight <= 0)
		{
			cardToSpawn = AffordableCards[i];
			break;
		}
	}

	if (cardToSpawn == nullptr)
	{
		std::cerr << "couldn't choose card to spawn";
		return false;
	}
	else
	{
		while (m_credits >= cardToSpawn->Cost)
		{
			m_enemyVector_ptr->push_back(cardToSpawn->createEnemy(glm::vec2(1.0f * (rand() % 1000), 2.0f * (rand() % 1000)), cardToSpawn->info));
			m_credits -= cardToSpawn->Cost;
		}
	}
	//After spawning indicate that spawning succeded
	return true;
}

void Director::Update(float dt)
{
	m_CurrentCooldown -= dt;
	
	m_credits += (*m_diffManager)->getCreditsperSecond() * dt;

	if (m_CurrentCooldown <= 0.0f)
	{
		//Try spawning
		bool didSpawn = this->Spawn();

		if (!didSpawn)
		{
			m_CurrentCooldown = m_SpawnCooldown;
		}
		else
		{
			//If couldn't spawn for some reason half the cooldown
			//Valid reason is for now non existent but should be lack of spawn spots
			m_CurrentCooldown = m_SpawnCooldown * 0.5f;
		}
	}

	
}