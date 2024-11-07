#pragma once
#include <glm.hpp>
#include"../src/Renderers/Renderer.h"
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN,
	GAME_LOSE,
	GAME_LVLUP
};
struct DebugInfo
{
	int CollisionChecks;
	int Enemies;
	int Projectiles;
	int PlayerHealth;
	int DrawCalls;
};

class Game
{
public:
	GameState State;
	bool Keys[1024];
	bool Mouse[2];

	DebugInfo debuginfo;

	glm::vec2 MousePos;
	unsigned int Width, Height;
	Game(unsigned int width, unsigned int height);
	~Game();
	void Init();
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
	void Collisions();
	void RenderDebug();
	void RenderLevelUp();
	void RenderLight();
private:
	Renderer renderer;
};