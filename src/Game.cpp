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
#include "Renderers/TextRenderer.h"
#include "ResourceHandlers/ResourceManager.h"

glm::vec2 ScreenCenter;

bool CheckCollision(GameObject& one, GameObject& two);
bool CheckCollisionWithPlayer(GameObject& one);
void Spawnxp(GameObject* enemy);

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_ACTIVE),Keys(),Width(width),Height(height),MousePos(0,0){}

SpriteRenderer* renderer;

PlayerRenderer* playerRenderer;

EnemyRenderer* enemyRenderer;

TextRenderer* textRenderer;

extern float MousePlayerAngle;


std::vector<std::shared_ptr<Enemy>> enemies;

Player* player;

extern glm::vec2 PlayerPosition = glm::vec2(0);

extern std::vector<std::shared_ptr<Projectile>> PlayerProjectiles;

std::vector<std::shared_ptr<GameObject>> expShards;

DebugInfo debuginfo;

Game::~Game()
{
	delete renderer;
	delete playerRenderer;
	delete player;
}

ImGuiWindowFlags flags = 0;

int lastlvl;

void Game::Init()
{
	//load shaders
	ResourceManager::LoadShader("src/Shaders/SpriteShader.vert", "src/Shaders/SpriteShader.frag", "sprite");
	ResourceManager::LoadShader("src/Shaders/TextShader.vert", "src/Shaders/TextShader.frag", "text");
	ScreenCenter = glm::vec2(this->Width / 2, this->Height / 2);

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

	ResourceManager::GetShader("sprite").Use();
	ResourceManager::GetShader("sprite").SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

	ResourceManager::GetShader("text").Use();
	ResourceManager::GetShader("text").SetMatrix4("projection", projection);

	ResourceManager::LoadTexture("src/Textures/512X512.png", false, "background");
	ResourceManager::LoadTexture("src/Textures/pizza.png", true, "pizza");
	ResourceManager::LoadTexture("src/Textures/player.png", true, "player");
	ResourceManager::LoadTexture("src/Textures/knife.png", true, "knife");
	ResourceManager::LoadTexture("src/Textures/WIDELEC.png", true, "fork");
	ResourceManager::LoadTexture("src/Textures/TASAK.png", true, "butcher");
	ResourceManager::LoadTexture("src/Textures/Tomato.png", true, "tomato");

	ResourceManager::LoadTexture("src/Textures/lvlup.png", true, "lvluphud");

	renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	playerRenderer = new PlayerRenderer(SpriteRenderer(ResourceManager::GetShader("sprite")));
	enemyRenderer = new EnemyRenderer(SpriteRenderer(ResourceManager::GetShader("sprite")), &PlayerPosition);
	textRenderer = new TextRenderer(ResourceManager::GetShader("text"));
	Texture2D PlayerSprite = ResourceManager::GetTexture("pizza");
	
	glm::vec2 StartingPlayerPosition = glm::vec2(static_cast<float>(this->Width) / 2 - PlayerSprite.Width, static_cast<float>(this->Height) / 2 - PlayerSprite.Height) + glm::vec2(30.0f,16.0f);

	player = new Player(StartingPlayerPosition, glm::vec2(PlayerSprite.Width, PlayerSprite.Height)*1.5f, PlayerSprite,&PlayerProjectiles,&PlayerPosition);

	flags |= ImGuiWindowFlags_AlwaysAutoResize;
	flags |= ImGuiWindowFlags_NoCollapse;

	player->weapons[0] = new KnifeWeapon("fork","knife",&player->stats,&PlayerPosition,1.0f);
	player->weapons[1] = new OrbitWeapon("knife", "Orbit", &player->stats, &PlayerPosition,5.0f);
	lastlvl = player->Level;
}

void Game::Render()
{
	renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f)-PlayerPosition, glm::vec2(this->Width, this->Height), 0.0f);
	
	for (auto xp : expShards)
	{
		xp->Draw(*enemyRenderer);
	}

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


	//textRenderer->RenderText("kkkk",  Common::ScreenSize.x / 2+20, Common::ScreenSize.y / 2, 1, glm::vec3(1.0f));
	player->Draw(*playerRenderer);
}

float spawnerTime;

void Game::Update(float dt)
{	
	//Menu
	if (this->Keys[GLFW_KEY_TAB])
	{
		if (this->State == GAME_ACTIVE)
			this->State = GAME_MENU;
		else
			this->State = GAME_ACTIVE;
	}
	if (!this->State == GAME_ACTIVE)
		return;
	if (!player->Alive)
		this->State = GAME_LOSE;

	if (player->Level > lastlvl)
	{
		this->State = GAME_LVLUP;
		lastlvl = player->Level;
		return;
	}

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
				ResourceManager::GetTexture("pizza"),
				player
			)));
		spawnerTime = 0.0f;
		
	}


	for (auto enemy : enemies)
	{
		enemy->Update(dt);
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
	if (this->State != GAME_ACTIVE)
		return;
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
				{
					player->Kills++;
					Spawnxp(enemy.get());

				}
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

	for (int i = 0; i < expShards.size();i++)
	{
		auto xp = expShards[i];
		debuginfo.CollisionChecks++;
		if (CheckCollisionWithPlayer(*xp))
		{
			player->GetXp(rand()%2+5);
			expShards.erase(expShards.begin() + i);
			
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
	ImGui::Text("Position : %f %f", PlayerPosition.x, PlayerPosition.y);

	ImGui::Spacing();
	ImGui::Text("Level %i", player->Level);
	ImGui::Text("xp %f/%f",player->xp,player->xpToLvl);

	ImGui::Spacing();
	ImGui::Text("Weapons : ");
	for (int i = 0; i < 6; i++)
	{

		ImGui::Text(player->weapons[i]->name.c_str());
	}

	ImGui::End();
	ImGui::Begin("Stat tweaker", (bool*)0, flags);
	ImGui::SliderFloat("Attack speed",&player->stats.AttackSpeed,1.0f, 25.0f);
	ImGui::SliderFloat("Movement speed", &player->stats.PlayerSpeed, 50.0f, 150.0f);
	ImGui::SliderInt("Projectile count", &player->stats.projectileCount, 1, 100);
	ImGui::End();
}

void Game::RenderLevelUp()
{
	Texture2D hudtxt = ResourceManager::GetTexture("lvluphud");
	glm::vec2 hudorigin = ScreenCenter - glm::vec2(hudtxt.Width, hudtxt.Height) * 0.5f;
	renderer->DrawSprite(hudtxt, hudorigin, glm::vec2(hudtxt.Width, hudtxt.Height));

	renderer->DrawSprite(ResourceManager::GetTexture("knife"), hudorigin + glm::vec2(197, 120), glm::vec2(64.0f, 64.0f));

	textRenderer->RenderText("knife", hudorigin.x + 280 ,hudorigin.y + 132, 1, glm::vec3(1.0f));
	//Debug exit
	if (this->Keys[GLFW_KEY_SPACE])
		this->State = GAME_ACTIVE;
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

void Spawnxp(GameObject* enemy)
{
	if ((rand() % 2))
	{
		expShards.push_back
		(
			std::make_shared<GameObject>(GameObject(
				enemy->Position,
				glm::vec2(32.0f, 32.0f),
				ResourceManager::GetTexture("tomato")

			))
		);
	}
}