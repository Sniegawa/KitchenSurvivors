#include "Game.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "SpriteRenderer.h"
#include "GameObject.h"
#include <iostream>
#include "Player.h"
#include "PlayerRenderer.h"
#include "EnemyRenderer.h"



Game::Game(unsigned int width, unsigned int height)
	: State(GAME_ACTIVE),Keys(),Width(width),Height(height){}

SpriteRenderer* renderer;

PlayerRenderer* playerRenderer;

EnemyRenderer* enemyRenderer;

Player* player;

GameObject* pizza;

glm::vec2 PlayerPosition;

float PlayerSize = 2.0f;
float PlayerSpeed = 75.0f;

Game::~Game()
{
	delete renderer;
	delete playerRenderer;
	delete pizza;
	delete player;
}

void Game::Init()
{
	//load shaders
	ResourceManager::LoadShader("shader/SpriteShader.vert", "shader/SpriteShader.frag", "sprite");



	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

	ResourceManager::GetShader("sprite").Use();
	ResourceManager::GetShader("sprite").SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

	ResourceManager::LoadTexture("textures/512X512.png", false, "background");
	ResourceManager::LoadTexture("textures/pizza.png", true, "pizza");
	ResourceManager::LoadTexture("textures/player.png", true, "player");

	renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	playerRenderer = new PlayerRenderer(SpriteRenderer(ResourceManager::GetShader("sprite")));
	enemyRenderer = new EnemyRenderer(SpriteRenderer(ResourceManager::GetShader("sprite")), &PlayerPosition);

	auto PlayerSprite = ResourceManager::GetTexture("player");
	
	glm::vec2 StartingPlayerPosition = glm::vec2(static_cast<float>(this->Width) / 2 - PlayerSprite.Width / 2, static_cast<float>(this->Height) / 2 - PlayerSprite.Height / 2);

	player = new Player(GameObject(StartingPlayerPosition, glm::vec2(PlayerSprite.Width, PlayerSprite.Height)*PlayerSize, PlayerSprite));
	
	glm::vec2 pizzaCoordinates = glm::vec2(10.0f, 10.0f);

	pizza = new GameObject(pizzaCoordinates, glm::vec2(64.0f), ResourceManager::GetTexture("pizza"));


}

void Game::Render()
{
	renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f)-PlayerPosition, glm::vec2(this->Width, this->Height), 0.0f);
	player->Draw(*playerRenderer);

	pizza->Draw(*enemyRenderer);
}

void Game::Update(float dt)
{	
	//Collisions
	//Lvl up
}

void Game::ProcessInput(float dt)
{
	if (this->State == GAME_ACTIVE)
	{
		float velocity = PlayerSpeed * dt;

		if (this->Keys[GLFW_KEY_A])
		{
			PlayerPosition.x -= velocity;
		}
		else if (this->Keys[GLFW_KEY_D])
		{
			PlayerPosition.x += velocity;
		}
		if (this->Keys[GLFW_KEY_W])
		{
			PlayerPosition.y -= velocity;
		}
		else if (this->Keys[GLFW_KEY_S])
		{
			PlayerPosition.y += velocity;
		}
	}
}
