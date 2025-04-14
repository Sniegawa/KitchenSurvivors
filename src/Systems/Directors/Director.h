#pragma once

class Enemy;

typedef struct SpawnCard
{
	Enemy* EnemyToSpawn;
	float Weight;
	int Cost;
} SpawnCard;

class Director
{
public:
	Director() {}

private:
	int m_credits;
	
};