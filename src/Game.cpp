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
#include "Weapons/ThrownWeapon.h"
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
bool MouseInRange(glm::vec2 MousePos, glm::vec2 start, glm::vec2 end);

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_ACTIVE),Keys(),Width(width),Height(height),MousePos(0,0){}

//DEBUG
static glm::vec4 randColor()
{
	glm::vec4 c;
	c = glm::vec4(glm::max(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), 0.1f), glm::max(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), 0.1f), glm::max(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), 0.1f), 1.0f);
	return c;
}

static float randFloat(float min = 0.0f, float max = 1.0f)
{
	return min + static_cast<float> (rand()) / static_cast<float> (RAND_MAX/(max-min));
}

SpriteRenderer* renderer;

PlayerRenderer* playerRenderer;

EnemyRenderer* enemyRenderer;

TextRenderer* textRenderer;

SpriteRenderer* UIRenderer;

extern float MousePlayerAngle;

Player* player;


struct Light
{
	glm::vec4 coords;// vec2 pos, float radius, float ??
	glm::vec4 color;
	Light(glm::vec4 _coords , glm::vec4 _color) : coords(_coords),color(_color)  {};
};

std::vector<Light> lights;


std::vector<std::shared_ptr<Enemy>> enemies;

extern glm::vec2 PlayerPosition = glm::vec2(0);

extern std::vector<std::shared_ptr<Projectile>> PlayerProjectiles;

std::vector<std::shared_ptr<GameObject>> expShards;

DebugInfo debuginfo;

Game::~Game()
{
	delete renderer;
	delete UIRenderer;
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
	ResourceManager::LoadShader("src/Shaders/UIShader.vert", "src/Shaders/UIShader.frag","UI");
	ScreenCenter = glm::vec2(this->Width / 2, this->Height / 2);

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	
	ResourceManager::GetShader("sprite").Use();
	ResourceManager::GetShader("sprite").SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

	ResourceManager::GetShader("text").Use();
	ResourceManager::GetShader("text").SetMatrix4("projection", projection);

	ResourceManager::GetShader("UI").Use();
	ResourceManager::GetShader("UI").SetInteger("image", 0);
	ResourceManager::GetShader("UI").SetMatrix4("projection", projection);


	ResourceManager::LoadTexture("src/Textures/512X512.png", false, "background");
	ResourceManager::LoadTexture("src/Textures/pizza.png", true, "pizza");
	ResourceManager::LoadTexture("src/Textures/player.png", true, "player");
	ResourceManager::LoadTexture("src/Textures/knife.png", true, "knife");
	ResourceManager::LoadTexture("src/Textures/WIDELEC.png", true, "fork");
	ResourceManager::LoadTexture("src/Textures/TASAK.png", true, "butcher");
	ResourceManager::LoadTexture("src/Textures/Tomato.png", true, "tomato");

	ResourceManager::LoadTexture("src/Textures/lvlup.png", true, "lvluphud");

	renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	UIRenderer = new SpriteRenderer(ResourceManager::GetShader("UI"));
	playerRenderer = new PlayerRenderer(SpriteRenderer(ResourceManager::GetShader("sprite")));
	enemyRenderer = new EnemyRenderer(SpriteRenderer(ResourceManager::GetShader("sprite")), &PlayerPosition);
	textRenderer = new TextRenderer(ResourceManager::GetShader("text"));
	Texture2D PlayerSprite = ResourceManager::GetTexture("pizza");
	
	glm::vec2 StartingPlayerPosition = glm::vec2(static_cast<float>(this->Width) / 2 - PlayerSprite.Width, static_cast<float>(this->Height) / 2 - PlayerSprite.Height) + glm::vec2(30.0f,16.0f);

	player = new Player(StartingPlayerPosition, glm::vec2(PlayerSprite.Width, PlayerSprite.Height)*1.5f, PlayerSprite,&PlayerProjectiles,&PlayerPosition);

	flags |= ImGuiWindowFlags_AlwaysAutoResize;
	flags |= ImGuiWindowFlags_NoCollapse;


	player->weapons[0] = new ThrownWeapon("fork","Fork","Throw a fork at enemy", &player->stats, &PlayerPosition, 1.0f);
	player->weapons[1] = new OrbitWeapon("knife", "Orbit", &player->stats, &PlayerPosition,5.0f);
	lastlvl = player->Level;
	
	for (int i = 0; i < 100; i++)
	{
		glm:: vec4 pos,col;
		pos = glm::vec4(randFloat(-2000,2000), randFloat(-2000,2000), randFloat(0.0001f,0.001f),randFloat(0.0001f,0.001f));
		col = randColor();
		col.w = randFloat(0.0f,4.0f); // ??
		lights.emplace_back(pos, col);
	}

	ResourceManager::GetShader("sprite").Use();

	unsigned int SSBOLights;
	glGenBuffers(1, &SSBOLights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOLights);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Light) * lights.size() , &lights[0], GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, SSBOLights, 0, sizeof(Light) * lights.size());


	glBindBufferBase(ResourceManager::GetShader("sprite").ID,1 , SSBOLights);
}


void Game::Render()
{
	ResourceManager::GetShader("sprite").SetVector2f("PlayerPos", PlayerPosition);
	renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f)-PlayerPosition, glm::vec2(this->Width*10.0f, this->Height*10.0f), 0.0f);
	
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
	
}

void Game::ProcessInput(float dt)
{
	if (this->Keys[GLFW_KEY_R])
	{
		player->Health = 100;
		player->Alive = true;
		this->State = GAME_ACTIVE;
	}
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
		std::string weapontxt = player->weapons[i]->name + " " + std::to_string(player->weapons[i]->level);
		ImGui::Text(weapontxt.c_str());
	}

	ImGui::End();
	ImGui::Begin("Stat tweaker", (bool*)0, flags);
	ImGui::SliderFloat("Attack speed",&player->stats.AttackSpeed,1.0f, 3.0f);
	ImGui::SliderFloat("Movement speed", &player->stats.PlayerSpeed, 50.0f, 250.0f);
	ImGui::SliderInt("Projectile count", &player->stats.projectileCount, 1, 10);
	ImGui::End();
}

void Game::RenderLevelUp()
{
	//This should be randomized so each slot has it's own weapon,
	//that is either an weapon from inventory or a new one
	Weapon* weapon = player->weapons[0];

	Texture2D hudtxt = ResourceManager::GetTexture("lvluphud");
	glm::vec2 hudorigin = ScreenCenter - glm::vec2(hudtxt.Width, hudtxt.Height) * 0.5f;
	UIRenderer->DrawSprite(hudtxt, hudorigin, glm::vec2(hudtxt.Width, hudtxt.Height));

	Texture2D knifetex = ResourceManager::GetTexture(weapon->sprite);

	UIRenderer->DrawSprite(knifetex, hudorigin + glm::vec2(197, 145) + glm::vec2(knifetex.Width, knifetex.Height)*0.5f, glm::vec2(knifetex.Width, knifetex.Height) * 1.25f);

	textRenderer->RenderText(weapon->name, hudorigin.x + 280 ,hudorigin.y + 132, .25f, glm::vec3(1.0f));
	textRenderer->RenderText(weapon->description, hudorigin.x + 280, hudorigin.y + 160, .2f, glm::vec3(1.0f));

	//Debug MousePos check according to the hud
	if (this->Keys[GLFW_KEY_L])
		printf("%f,%f\n", this->MousePos.x - hudorigin.x, this->MousePos.y - hudorigin.y);
	
	//Click
	if (this->Keys[GLFW_KEY_1]) {
		weapon->LvlUp();
		this->State = GAME_ACTIVE;
	}
	else if (this->Keys[GLFW_KEY_2])
	{

	}
	else if (this->Keys[GLFW_KEY_3])
	{

	}
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

bool MouseInRange(glm::vec2 MousePos,glm::vec2 start, glm::vec2 end)
{
	return 
		start.x <= MousePos.x &&
		MousePos.x <= end.x && 
		start.y <= MousePos.y &&
		MousePos.y <= end.y;

}