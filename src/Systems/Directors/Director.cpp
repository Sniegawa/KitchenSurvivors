#include "Director.h"
#include "Objects/Enemy.h"

#include <algorithm>
#include <random>

Director::Director(unsigned int* _seed)
{
	m_seed = _seed;

	m_SpawnCards =
	{
		{nullptr,10,25},
		{nullptr,10,20},
		{nullptr,3,200},
		{nullptr,1,500},
		{nullptr,15,10},
		{nullptr,15,15}
	};

	m_credits = 400;
}

void Director::Spawn()
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

	if (AffordableCards.size() <= 0)
	{
		//Delay spawn a bit
		m_CurrentCooldown += m_SpawnCooldown * 0.25f;
		return;
	}

	//Shuffle affordable cards for random spawning
	auto rng = std::default_random_engine{*m_seed};
	std::shuffle(std::begin(AffordableCards), std::end(AffordableCards), rng);

	int randomWeight = rand() % maxWeight;

	SpawnCard* cardToSpawn;

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
	}

}