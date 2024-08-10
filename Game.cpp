#include "Game.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "SpriteRenderer.h"
#include "GameObject.h"
#include <iostream>
#include<vector>
#include "Player.h"
#include "PlayerRenderer.h"
#include "EnemyRenderer.h"
#include "Projectile.h"

glm::vec2 ScreenCenter;

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_ACTIVE),Keys(),Width(width),Height(height),MousePos(0,0){}

SpriteRenderer* renderer;

PlayerRenderer* playerRenderer;

EnemyRenderer* enemyRenderer;

Player* player;

GameObject* pizza;

glm::vec2 PlayerPosition;

float PlayerSize = 2.0f;
float PlayerSpeed = 75.0f;

std::vector<Projectile*> projectiles;

glm::vec2 mouseDir;

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

	ScreenCenter = glm::vec2(this->Width / 2, this->Height / 2);

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

	auto PlayerSprite = ResourceManager::GetTexture("pizza");
	
	glm::vec2 StartingPlayerPosition = glm::vec2(static_cast<float>(this->Width) / 2 - PlayerSprite.Width, static_cast<float>(this->Height) / 2 - PlayerSprite.Height);

	player = new Player(StartingPlayerPosition, glm::vec2(PlayerSprite.Width, PlayerSprite.Height)*PlayerSize, PlayerSprite);
	
	glm::vec2 pizzaCoordinates = glm::vec2(10.0f, 10.0f);

	pizza = new GameObject(pizzaCoordinates, glm::vec2(64.0f), ResourceManager::GetTexture("pizza"));
}

void Game::Render()
{
	renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f)-PlayerPosition, glm::vec2(this->Width, this->Height), 0.0f);
	pizza->Draw(*enemyRenderer);

	//TODO: gpu instancing
	for(GameObject* obj : projectiles)
	{
		obj->Draw(*enemyRenderer);
	}

	player->Draw(*playerRenderer);
}

float c;

void Game::Update(float dt)
{	
	
	for (Projectile* obj : projectiles)
	{
		obj->UpdatePosition(dt);
	}
	pizza->Position = glm::vec2(sin(c), cos(c)) * 200.0f;

	c += dt;
	//Menu
	if (this->Keys[GLFW_KEY_TAB])
	{
		if (this->State == GAME_ACTIVE)
			this->State = GAME_MENU;
		else
			this->State = GAME_ACTIVE;
	}

	player->ReduceCooldowns(dt);
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
		//Test Strzelania
		if (this->Keys[GLFW_KEY_E] && player->CanShoot)
		{
			//K¹t myszka/gracz
			float angle = -atan2(this->MousePos.x - ScreenCenter.x, this->MousePos.y - ScreenCenter.y);
			
			glm::vec2 position;

			position = glm::normalize(MousePos - ScreenCenter);
			
			projectiles.push_back(
				new Projectile(
					PlayerPosition + ScreenCenter + position * 20.0f,
					glm::vec2(32.0f),
					ResourceManager::GetTexture("pizza"),
					glm::vec3(1.0f),
					angle,
					250,
					position
				)
			);

			player->Shoot();
		}
	}
}
