#pragma once
#include <glm.hpp>
#include "Renderers/Renderer.h"
#include "Systems/CookingMenu/CookingMenu.h"
#include "Systems/Directors/Director.h"
#include "Systems/Directors/DifficultyManager.h"
#include "Objects/Player.h"
#include "Renderers/Camera.h"

enum GameState 
{
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
	Camera* camera;
	Director* director;
	DifficultyManager* diffManager;

	glm::vec2 MousePos;
	unsigned int Width, Height;//Useless
	Game(unsigned int width, unsigned int height);
	~Game();
	void Init();
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
	void Collisions();
	void RenderDebug();
	void RenderLevelUp();
	void RenderUI();
private:
	Renderer renderer;
	CookingMenu cookingMenu;
	void LoadTextures();
	void LoadShaders();
	bool CheckCollisionWithPlayer(GameObject& one);

	unsigned int seed;

	std::vector<std::shared_ptr<Enemy>> enemies;
};