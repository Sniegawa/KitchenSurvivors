#pragma once
struct PlayerStats {
	float AttackSpeed = 1.0f;
	int MaxHealth = 100;
	int projectileCount = 1;
	float PlayerSpeed = 75.0f;
};

static glm::vec2 ScreenSize = glm::vec2(1280, 720);