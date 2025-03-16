#pragma once
#include <glm.hpp>
#include"../src/Renderers/Renderer.h"
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN,
	GAME_LOSE,
	GAME_LVLUP,
};


class Game
{
public:
	GameState State;
	bool Keys[1024];
	bool Mouse[2];
	bool isCooking = false;
	Player* player;

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
	void RenderUI();
private:
	Renderer renderer;
	CookingMenu cookingMenu;
	void LoadTextures();
	void LoadShaders();
	void InnitItems();
	bool CheckCollisionWithPlayer(GameObject& one);
};