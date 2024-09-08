#pragma once
#include <unordered_map>
struct PlayerStats {
	float AttackSpeed = 1.0f;
	int MaxHealth = 100;
	int projectileCount = 1;
	float PlayerSpeed = 75.0f;
	float ExpirienceMultiplier = 1.0f;
};

static glm::vec2 ScreenSize = glm::vec2(1280, 720);

static std::unordered_map<int, int> lvlmap
{
	{1,10},
	{2,20},
	{3,25}
};