#include "Game.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"

#include "SpriteRenderer.h"
#include "PlayerRenderer.h"
#include "EnemyRenderer.h"

#include "Projectile.h"

#include "Weapon.h"

glm::vec2 ScreenCenter;


bool CheckCollision(GameObject& one, GameObject& two);

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_ACTIVE),Keys(),Width(width),Height(height),MousePos(0,0){}

SpriteRenderer* renderer;

PlayerRenderer* playerRenderer;

EnemyRenderer* enemyRenderer;

Player* player;

std::vector<std::shared_ptr<Enemy>> enemies;

std::shared_ptr<Enemy> pizza;

glm::vec2 PlayerPosition;

float PlayerSize = 2.0f;


std::vector<std::unique_ptr<Projectile>> PlayerProjectiles;

glm::vec2 mouseDir;

DebugInfo debuginfo;


Game::~Game()
{
	delete renderer;
	delete playerRenderer;
	delete player;
}

ImGuiWindowFlags flags = 0;


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
	ResourceManager::LoadTexture("textures/knife.png", true, "knife");

	renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	playerRenderer = new PlayerRenderer(SpriteRenderer(ResourceManager::GetShader("sprite")));
	enemyRenderer = new EnemyRenderer(SpriteRenderer(ResourceManager::GetShader("sprite")), &PlayerPosition);

	auto PlayerSprite = ResourceManager::GetTexture("pizza");
	
	glm::vec2 StartingPlayerPosition = glm::vec2(static_cast<float>(this->Width) / 2 - PlayerSprite.Width, static_cast<float>(this->Height) / 2 - PlayerSprite.Height);

	player = new Player(StartingPlayerPosition, glm::vec2(PlayerSprite.Width, PlayerSprite.Height)*PlayerSize, PlayerSprite,&PlayerProjectiles);
	
	glm::vec2 pizzaCoordinates = glm::vec2(10.0f, 10.0f);

	enemies.push_back(std::make_shared<Enemy>(Enemy(pizzaCoordinates, glm::vec2(64.0f), ResourceManager::GetTexture("pizza"))));
	enemies.push_back(std::make_shared<Enemy>(Enemy(pizzaCoordinates + glm::vec2(100.0f,0.0f), glm::vec2(64.0f), ResourceManager::GetTexture("pizza"))));
	enemies.push_back(std::make_shared<Enemy>(Enemy(pizzaCoordinates + glm::vec2(200.0f, 0.0f), glm::vec2(64.0f), ResourceManager::GetTexture("pizza"))));

	flags |= ImGuiWindowFlags_AlwaysAutoResize;
	flags |= ImGuiWindowFlags_NoCollapse;
}

void Game::Render()
{
	renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f)-PlayerPosition, glm::vec2(this->Width, this->Height), 0.0f);
	
	for (int i = 0; i < enemies.size(); i++)
	{
		if (enemies[i]->isDead)
		{
			
			enemies.erase(enemies.begin() + i);
			continue;
		}
		enemies[i]->Draw(*enemyRenderer);
	}

	//TODO: gpu instancing
	for(auto const& obj : PlayerProjectiles)
	{
		obj->Draw(*enemyRenderer);
	}

	player->Draw(*playerRenderer);
}

float spawnerTime;

float WeaponTimer = 0.0f;


void Game::Update(float dt)
{	
	
	if (!player->Alive)
		this->State = GAME_LOSE;


	WeaponTimer += dt;
	if (WeaponTimer >= 1.0f / player->stats.AttackSpeed)
	{
		float angle = -atan2(this->MousePos.x - ScreenCenter.x, this->MousePos.y - ScreenCenter.y);

		Texture2D knifetex = ResourceManager::GetTexture("knife");

		int projectileCount = player->stats.projectileCount;
	
		if (projectileCount > 1) {

			if (projectileCount % 2 == 0 && projectileCount < 25)
			{
				for (int i = -projectileCount/2; i < projectileCount/2 + 1; i++)
				{
					if (i == 0)
						continue;
					float angleoffset = (glm::pi<float>() / 12) * i;
					PlayerProjectiles.push_back(
						std::make_unique<Projectile>(Projectile(
							PlayerPosition + ScreenCenter,
							glm::vec2(knifetex.Width, knifetex.Height),
							knifetex,
							25.0f,
							glm::vec3(1.0f),
							angle + glm::pi<float>() / 2.0f + angleoffset,
							500,
							glm::vec2(-sin(angle + angleoffset), cos(angle + angleoffset))
						))
					);
				}
			}
			else
			{
				for (int i = -projectileCount / 2; i < projectileCount / 2 + 1; i++)
				{
					float angleoffset = (glm::pi<float>() / 12) * i;
					PlayerProjectiles.push_back(
						std::make_unique<Projectile>(Projectile(
							PlayerPosition + ScreenCenter,
							glm::vec2(knifetex.Width, knifetex.Height),
							knifetex,
							25.0f,
							glm::vec3(1.0f),
							angle + glm::pi<float>() / 2.0f + angleoffset,
							500,
							glm::vec2(-sin(angle + angleoffset), cos(angle + angleoffset))
						))
					);
				}
			}
		}
		else 
		{
			PlayerProjectiles.push_back(
				std::make_unique<Projectile>(Projectile(
					PlayerPosition + ScreenCenter,
					glm::vec2(knifetex.Width, knifetex.Height),
					knifetex,
					25.0f,
					glm::vec3(1.0f),
					angle + glm::pi<float>() / 2.0f,
					500,
					glm::vec2(-sin(angle), cos(angle))
				))
			);
		}
		WeaponTimer = 0.0f;
	}


	for (int i = 0; i < PlayerProjectiles.size();i++)
	{
		auto obj = PlayerProjectiles[i].get();
		if (obj->IsDead())
		{
			PlayerProjectiles.erase(PlayerProjectiles.begin() + i);
			continue;
		}
		obj->Update(dt);
	}
	
	spawnerTime += dt;

	if (spawnerTime >= 0.5f)
	{
		enemies.push_back(std::make_shared<Enemy>(
			Enemy(
				glm::vec2(rand()%500,rand()%500),
				glm::vec2(64.0f),
				ResourceManager::GetTexture("pizza")
			)));
		spawnerTime = 0.0f;
		
	}


	for (auto enemy : enemies)
	{
		enemy->Update(dt);
	}

	//Menu
	if (this->Keys[GLFW_KEY_TAB])
	{
		if (this->State == GAME_ACTIVE)
			this->State = GAME_MENU;
		else
			this->State = GAME_ACTIVE;
	}

	debuginfo.Enemies = enemies.size();
	debuginfo.Projectiles = PlayerProjectiles.size();
	debuginfo.PlayerHealth = player->Health;
	//Lvl up
}
void Game::ProcessInput(float dt)
{
	if (this->State == GAME_ACTIVE)
	{
		float velocity = player->stats.PlayerSpeed * dt;

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

void Game::Collisions()
{
	debuginfo.CollisionChecks = 0;
	for (int i = 0; i < enemies.size();i++)
	{
		auto enemy = enemies[i];

		for (auto const& projectile : PlayerProjectiles)
		{
			debuginfo.CollisionChecks++;
			if (CheckCollision(*projectile, *enemy))
			{
				enemy->TakeDamage(projectile->DamageDealt);
				projectile->Hit();
				if (enemy->Health <= 0.0f)
					player->Kills++;
			}
		}
		debuginfo.CollisionChecks++;
		/* nie dodajemy player position w przypadku playera do naprawy
		if (CheckCollision(*enemy, *player))
		{
			enemy->TakeDamage(1.0f);
			player->TakeDamage(1.0f);
		}*/
		for (int j = 0; j < enemies.size(); j++)
		{
			debuginfo.CollisionChecks++;
			if (i == j)
				continue;
			if (CheckCollision(*enemies[i], *enemies[j]))
			{
				glm::vec2 test(glm::normalize(enemies[i]->Position - enemies[j]->Position));
				enemy->Position += test;
			}
		}
	}
	
}

void Game::RenderDebug()
{
	ImGui::Begin("Debug info",(bool*)0,flags);
	ImGui::Text("HP : %i", debuginfo.PlayerHealth);
	ImGui::Text("Projectiles : %i", debuginfo.Projectiles);
	ImGui::Text("Enemies : %i", debuginfo.Enemies);
	ImGui::Text("CollisionChecks : %i", debuginfo.CollisionChecks);
	ImGui::Text("Kills : %i",player->Kills);
	ImGui::End();
	ImGui::Begin("Stat tweaker", (bool*)0, flags);
	ImGui::SliderFloat("Attack speed",&player->stats.AttackSpeed,1.0f, 5.0f);
	ImGui::SliderFloat("Movement speed", &player->stats.PlayerSpeed, 50.0f, 150.0f);
	ImGui::End();
}

bool CheckCollision(GameObject& one, GameObject& two)
{
	bool axisX = one.Position.x + one.Size.x >= two.Position.x && two.Position.x + two.Size.x >= one.Position.x;
	bool axisY = one.Position.y + one.Size.y >= two.Position.y && two.Position.y + two.Size.y >= one.Position.y;
	
	return axisX && axisY;
}