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

#include "Objects/GameObject.h"
#include "Objects/Player.h"
#include "Objects/Enemy.h"
#include "Objects/Projectile.h"

#include "Weapons/Weapon.h"
#include "Weapons/Knife.h"
#include "Weapons/OrbitWeapon.h"

#include "Renderers/PlayerRenderer.h"
#include "Renderers/EnemyRenderer.h"
#include "Renderers/SpriteRenderer.h"
#include "ResourceHandlers/ResourceManager.h"

glm::vec2 ScreenCenter;

bool CheckCollision(GameObject& one, GameObject& two);
bool CheckCollisionWithPlayer(GameObject& one);

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_ACTIVE),Keys(),Width(width),Height(height),MousePos(0,0){}

SpriteRenderer* renderer;

PlayerRenderer* playerRenderer;

EnemyRenderer* enemyRenderer;

extern float MousePlayerAngle;


std::vector<std::shared_ptr<Enemy>> enemies;

Player* player;

extern glm::vec2 PlayerPosition = glm::vec2(0);

extern std::vector<std::shared_ptr<Projectile>> PlayerProjectiles;

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
	ResourceManager::LoadShader("src/Shaders/SpriteShader.vert", "src/Shaders/SpriteShader.frag", "sprite");
	
	ScreenCenter = glm::vec2(this->Width / 2, this->Height / 2);

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

	ResourceManager::GetShader("sprite").Use();
	ResourceManager::GetShader("sprite").SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

	ResourceManager::LoadTexture("src/Textures/512X512.png", false, "background");
	ResourceManager::LoadTexture("src/Textures/pizza.png", true, "pizza");
	ResourceManager::LoadTexture("src/Textures/player.png", true, "player");
	ResourceManager::LoadTexture("src/Textures/knife.png", true, "knife");
	ResourceManager::LoadTexture("src/Textures/WIDELEC.png", true, "fork");
	ResourceManager::LoadTexture("src/Textures/TASAK.png", true, "butcher");

	renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	playerRenderer = new PlayerRenderer(SpriteRenderer(ResourceManager::GetShader("sprite")));
	enemyRenderer = new EnemyRenderer(SpriteRenderer(ResourceManager::GetShader("sprite")), &PlayerPosition);

	Texture2D PlayerSprite = ResourceManager::GetTexture("pizza");
	
	glm::vec2 StartingPlayerPosition = glm::vec2(static_cast<float>(this->Width) / 2 - PlayerSprite.Width, static_cast<float>(this->Height) / 2 - PlayerSprite.Height) + glm::vec2(30.0f,16.0f);

	player = new Player(StartingPlayerPosition, glm::vec2(PlayerSprite.Width, PlayerSprite.Height)*1.5f, PlayerSprite,&PlayerProjectiles);
	
	glm::vec2 pizzaCoordinates = glm::vec2(10.0f, 10.0f);

	flags |= ImGuiWindowFlags_AlwaysAutoResize;
	flags |= ImGuiWindowFlags_NoCollapse;

	

	player->weapons[0] = new KnifeWeapon("fork","knife",&player->stats,&PlayerPosition,1.0f);
	player->weapons[1] = new OrbitWeapon("knife", "Orbit", &player->stats, &PlayerPosition,5.0f);


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

void Game::Update(float dt)
{	
	
	if (!player->Alive)
		this->State = GAME_LOSE;


	for (auto weapon : player->weapons)
	{
		weapon->Update(dt);
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

	player->UpdateCooldowns(dt);

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
	MousePlayerAngle = -atan2(this->MousePos.x - ScreenCenter.x, this->MousePos.y - ScreenCenter.y);
}

//Projectile deaths are framerate dependent FIX
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
		
		if (CheckCollisionWithPlayer(*enemy))
		{
			enemy->TakeDamage(2.0f);
			player->TakeDamage(1.0f);
		}
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
	ImGui::Text("Weapons : ");
	for (int i = 0; i < 6; i++)
	{

		ImGui::Text(player->weapons[i]->name.c_str());
	}

	ImGui::End();
	ImGui::Begin("Stat tweaker", (bool*)0, flags);
	ImGui::SliderFloat("Attack speed",&player->stats.AttackSpeed,1.0f, 5.0f);
	ImGui::SliderFloat("Movement speed", &player->stats.PlayerSpeed, 50.0f, 150.0f);
	ImGui::SliderInt("Projectile count", &player->stats.projectileCount, 1, 5);
	ImGui::End();
}

bool CheckCollision(GameObject& one, GameObject& two)
{
	bool axisX = one.Position.x + one.Size.x >= two.Position.x && two.Position.x + two.Size.x >= one.Position.x;
	bool axisY = one.Position.y + one.Size.y >= two.Position.y && two.Position.y + two.Size.y >= one.Position.y;
	
	return axisX && axisY;
}

bool CheckCollisionWithPlayer(GameObject& one)
{
	GameObject two = *player;
	two.Position += PlayerPosition;
	bool axisX = one.Position.x + one.Size.x >= two.Position.x && two.Position.x + two.Size.x >= one.Position.x;
	bool axisY = one.Position.y + one.Size.y >= two.Position.y && two.Position.y + two.Size.y >= one.Position.y;

	return axisX && axisY;
}