#include "Director.h"
#include "Objects/Enemy.h"

#include <algorithm>
#include <random>
#include <functional>

//#include "Objects/Enemies/EnemyIncludes.h"

//Some clever usage of modern c++ need to study it some more
//std::function to store it as lambda in SpawnCard
template <typename T>
std::function<std::shared_ptr<Enemy>(const glm::vec2&)> createEnemyFactory()
{
	return [](const glm::vec2& pos) -> std::shared_ptr<Enemy>
		{
			std::shared_ptr<T> enemy = std::make_shared<T>();
			enemy->SetPosition(pos);
			return enemy;
		};
}

Director::Director(unsigned int* _seed, DifficultyManager** _diffManager, std::vector<std::shared_ptr<Enemy>>* enemies_ptr)
{
	m_enemyVector_ptr = enemies_ptr;
	m_diffManager = _diffManager;
	m_seed = _seed;
	m_CurrentCooldown = m_SpawnCooldown;

	m_SpawnCards =
	{
		{10,25,createEnemyFactory<Enemy>(),{glm::vec2(15.0f),}}
	};

	m_credits = 400;
}

bool Director::Spawn()
{
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
			m_enemyVector_ptr->push_back(cardToSpawn->createEnemy(glm::vec2(1.0f,0.0f)));
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
		bool didSpawn = this->Spawn();

		if (!didSpawn)
		{
			m_CurrentCooldown = m_SpawnCooldown;
		}
		else
		{
			m_CurrentCooldown = m_SpawnCooldown * 0.5f;
		}
	}

	
}