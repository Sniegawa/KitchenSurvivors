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

#include "ResourceHandlers/ResourceManager.h"
#include "Systems/CookingMenu/CookingMenu.h"

#include "Renderers/Renderer.h"

#include "Objects/GameObject.h"
#include "Objects/Player.h"
#include "Objects/Enemy.h"
#include "Objects/Projectile.h"

#include "Weapons/ThrownWeapon.h"
#include "Weapons/OrbitWeapon.h"

glm::vec2 ScreenCenter;

bool CheckCollision(GameObject& one, GameObject& two);
void Spawnxp(GameObject* enemy);
bool MouseInRange(glm::vec2 MousePos, glm::vec2 start, glm::vec2 end);

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height), MousePos(0, 0),
	camera(new Camera()),
	seed(12345),
	diffManager(new DifficultyManager(2.0f, 5.0f)),
	director(new Director(&this->seed,&this->diffManager,&this->enemies))
{}

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


GameObject* background;

std::vector<pointLight> lights;

extern std::vector<std::shared_ptr<Projectile>> PlayerProjectiles;

std::vector<std::shared_ptr<GameObject>> expShards;

Game::~Game()
{
	delete player;
	delete camera;
	delete director;
}

ImGuiWindowFlags flags = 0;

int lastlvl;

float k1, k2;

glm::mat4 view;

void Game::LoadTextures()
{
	ResourceManager::LoadTexture("src/Textures/512X512.png", false, "background");
	ResourceManager::LoadTexture("src/Textures/pizza.png", true, "pizza");
	ResourceManager::LoadTexture("src/Textures/pizza_n.png", false, "pizza_n");
	ResourceManager::LoadTexture("src/Textures/player.png", true, "player");
	ResourceManager::LoadTexture("src/Textures/knife.png", true, "knife");
	ResourceManager::LoadTexture("src/Textures/WIDELEC.png", true, "fork");
	ResourceManager::LoadTexture("src/Textures/TASAK.png", true, "butcher");
	ResourceManager::LoadTexture("src/Textures/Tomato.png", true, "tomato");
	ResourceManager::LoadTexture("src/Textures/Salt.png", true, "salt");
	ResourceManager::LoadTexture("src/Textures/Pepper.png", true, "pepper");
	ResourceManager::LoadTexture("src/Textures/lvlup.png", true, "lvluphud");
	ResourceManager::LoadTexture("src/Textures/BurgerBase.png", true, "BurgerBase");
	ResourceManager::LoadTexture("src/Textures/burger-mold.png", true, "BurgerMold");
}

void Game::LoadShaders()
{
	ResourceManager::LoadShader("src/Shaders/SpriteShader.vert", "src/Shaders/SpriteShader.frag", "sprite");
	ResourceManager::LoadShader("src/Shaders/InstancedSpriteShader.vert", "src/Shaders/InstancedSpriteShader.frag", "instancedSprite");
	ResourceManager::LoadShader("src/Shaders/TextShader.vert", "src/Shaders/TextShader.frag", "text");
	ResourceManager::LoadShader("src/Shaders/UIShader.vert", "src/Shaders/UIShader.frag", "UI");
	ResourceManager::LoadShader("src/Shaders/DefferedLight.vert", "src/Shaders/DefferedLight.frag", "light");
	ResourceManager::LoadShader("src/Shaders/CookingMenu.vert", "src/Shaders/CookingMenu.frag", "CookingMenu");
	ResourceManager::LoadShader("src/Shaders/LineShader.vert", "src/Shaders/LineShader.frag", "Line");
	ResourceManager::LoadComputeShader("src/Shaders/ComputeShaders/Lightmap.cmpt", "Lightmap");
	ResourceManager::LoadComputeShader("src/Shaders/ComputeShaders/Downscaling.cmpt", "Downscaling");
	ResourceManager::LoadComputeShader("src/Shaders/ComputeShaders/NormalCalculation.cmpt", "Normals");
}


void Game::Init()
{
	this->LoadShaders();
	this->LoadTextures();
	this->renderer.RendererSetup();
	this->renderer.MousePos = &this->MousePos;
	
	Texture2D& PlayerSprite = ResourceManager::GetTexture("pizza");
	glm::vec2 playerCenter = glm::vec2(static_cast<float>(this->Width) / 2, static_cast<float>(this->Height) / 2) - (glm::vec2(PlayerSprite.Width, PlayerSprite.Height) * 1.5f) * 0.5f;// +glm::vec2(30.0f, 16.0f);
	player = new Player(playerCenter, glm::vec2(PlayerSprite.Width, PlayerSprite.Height) * 1.5f, &PlayerSprite, ResourceManager::GetShaderPtr("sprite"), PLAYER, &PlayerProjectiles);

	this->camera->Setup(this->player);
	
	//Debug UI flags
	flags |= ImGuiWindowFlags_AlwaysAutoResize;
	flags |= ImGuiWindowFlags_NoCollapse;

	ScreenCenter = glm::vec2(this->Width / 2, this->Height / 2);

	background = new GameObject(glm::vec2(0.0), glm::vec2(this->Width * 10.0f, this->Height * 10.0f), 0.0f, &ResourceManager::GetTexture("background"), ResourceManager::GetShaderPtr("sprite"), BACKGROUND);

	for (int i = 0; i < 6; i++)
	{
		player->weapons[i] = new Weapon();
	}

	this->cookingMenu.InnitCookingMenu(player);

	player->inventory.addIngredient(this->cookingMenu.INGREDIENTS.at(0), 2); // Bread
	player->inventory.addIngredient(this->cookingMenu.INGREDIENTS.at(1), 2); // Milk
	player->inventory.addIngredient(this->cookingMenu.INGREDIENTS.at(2), 1); // Garlic
	player->inventory.addIngredient(this->cookingMenu.INGREDIENTS.at(3), 1); // Onion
	player->inventory.addIngredient(this->cookingMenu.INGREDIENTS.at(4), 1); // Tomato
	player->inventory.addIngredient(this->cookingMenu.INGREDIENTS.at(5), 1); // Chili
	player->inventory.addIngredient(this->cookingMenu.INGREDIENTS.at(6), 1); // Salt
	player->inventory.addIngredient(this->cookingMenu.INGREDIENTS.at(7), 10); // Pepper

	player->weapons[0] = new ThrownWeapon("fork","Fork","Throw a fork at enemy", &player->stats, player, 1.0f);
	player->weapons[1] = new OrbitWeapon("knife", "Orbit", &player->stats, player,5.0f);
	lastlvl = player->Level;

	srand(time(0));
	for (int i = 0; i < 100; i++)
	{
		glm::vec4 pos;
		glm::vec3 col;
		float k1, k2, intensity;
		int random = rand() % 3;
		if (random == 0)
		{
			k1 = 0.01;
			k2 = 0.001;
			intensity = 2.0;
		}
		else if( random == 1)
		{
			k1 = 0.3;
			k2 = 0.1;
			intensity = 1.0;
		}
		else {
			k1 = 0.1;
			k2 = 0.01;
			intensity = 1.5;
		}
		pos = glm::vec4(randFloat(0,2000), randFloat(0,2000),k1*8, k2*8);
		col = glm::vec3(randColor());
		lights.emplace_back(pos, glm::vec4(col.x,col.y,col.z, intensity));
	}
	lights[0].coords.x = 4;
	lights[0].coords.y = 4;

	ResourceManager::GetShader("light").Use();

	unsigned int SSBOLights;
	glGenBuffers(1, &SSBOLights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOLights);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(pointLight) * lights.size(), lights.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, SSBOLights, 0, sizeof(pointLight) * lights.size());

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSBOLights);

}

void Game::Render()
{
	Common::debuginfo.DrawCalls = 0;
	
	this->renderer.RenderBackground(background);
	
	std::vector<GameObject*> RenderData;
	RenderData.reserve(sizeof(GameObject*) * (enemies.size() + expShards.size() + PlayerProjectiles.size()));
	for (const auto& enemy : enemies)
	{
		//if (glm::distance(player->GetPosition(), enemy->GetPosition()) > 800.0f)
			//continue;

		RenderData.push_back(enemy.get());
		
	}

	for (const auto& expShard : expShards)
	{
		if (glm::distance(player->GetPosition(), expShard->GetPosition()) > 800.0f)
			continue;
		RenderData.push_back(expShard.get());
	}

	for (const auto& projectile : PlayerProjectiles)
	{
		if (glm::distance(player->GetPosition(), projectile->GetPosition()) > 800.0f)
			continue;
		RenderData.push_back(projectile.get());
	}

	this->renderer.Render(RenderData);
	this->renderer.RenderPlayer(player);
	this->renderer.RenderLight();
}

void Game::RenderUI()
{
	if (this->isCooking)
		this->renderer.RenderCookingMenu(&this->player->inventory,this->cookingMenu);
}

void Game::Update(float dt)
{	
	if (this->Keys[GLFW_KEY_TAB])
	{
		if (this->State == GAME_ACTIVE)
			this->State = GAME_MENU;
		else
			this->State = GAME_ACTIVE;
	}

	if (!player->Alive)
		this->State = GAME_LOSE;

	if (!this->State == GAME_ACTIVE)
		return;

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

	for (auto enemy : enemies)
	{
		enemy->Update(dt);
		enemy->NavigateToPlayer(player, dt);
	}

	for (int i = 0; i < enemies.size(); i++)
	{
		if (enemies[i]->isDead())
		{
			enemies.erase(enemies.begin() + i);
			continue;
		}
	}

	this->diffManager->Update(dt);
	this->director->Update(dt);
	player->UpdateCooldowns(dt);

	if (player->inventory.ChangedState)
	{
		this->cookingMenu.UpdateCookingMenu();
		player->inventory.ChangedState = false;
	}
	
	this->camera->Update(dt);

	Common::debuginfo.Enemies = enemies.size();
	Common::debuginfo.Projectiles = PlayerProjectiles.size();
	Common::debuginfo.PlayerHealth = player->GetHealth();
	
}

float temptime;
float cookingcooldown;
float pausecd;
float selectcd;
void Game::ProcessInput(float dt)
{
	pausecd -= dt;
	temptime -= dt;
	selectcd -= dt;
	if (this->Keys[GLFW_KEY_TAB] && pausecd <= 0.0f)
	{
		this->State = this->State == GAME_MENU ? GAME_ACTIVE : GAME_MENU;
		pausecd = 0.02f;
		return;
	}

	if (this->State == GAME_ACTIVE)
	{
		float velocity = player->stats.PlayerSpeed * dt;
		glm::vec2 VectorToMove = glm::vec2(0.0f);
		if (this->Keys[GLFW_KEY_A])
		{
			VectorToMove.x -= velocity;
		}
		else if (this->Keys[GLFW_KEY_D])
		{
			VectorToMove.x += velocity;
		}
		if (this->Keys[GLFW_KEY_W])
		{
			VectorToMove.y -= velocity;
		}
		else if (this->Keys[GLFW_KEY_S])
		{
			VectorToMove.y += velocity;
		}

		player->Move(VectorToMove);

		if (isCooking)
		{
			cookingcooldown -= (dt * 4.0f);
			//Select recipe
			if ((this->Mouse[GLFW_MOUSE_BUTTON_1] || this->Keys[GLFW_KEY_E]) && selectcd <= 0.0f)
			{
				this->cookingMenu.SelectHoveredSlot();
				selectcd = 0.05f;
			}
			//Cook
			if (this->Keys[GLFW_KEY_SPACE])
			{
				this->cookingMenu.Cook();
				isCooking = false;
				cookingcooldown = 0.0f;
			}
		}
		else
		{
			cookingcooldown -= dt;
		}

		if (cookingcooldown <= 0)
		{
			this->isCooking = false;
			this->cookingMenu.ResetSlots();
			//this->renderer.ResetCookingMenuInfo(); if it is important implement it in CookingMenu
		}

		if (this->Keys[GLFW_KEY_C] && cookingcooldown <= 1.75f)
		{
			this->isCooking = true;
			cookingcooldown = 5.0f;

		}

		if (this->Keys[GLFW_KEY_P] && temptime <= 0.0f)
		{
			temptime = 2.0f;
			this->player->AddEffect(this->cookingMenu.EFFECTS.at(0), 2.0f);

		}
	}
	Common::MousePlayerAngle = -atan2(this->MousePos.x - ScreenCenter.x, this->MousePos.y - ScreenCenter.y);
}

void Game::Collisions()
{

	if (this->State != GAME_ACTIVE)
		return;
	Common::debuginfo.CollisionChecks = 0;

	std::shared_ptr<Enemy> enemy;

	for (int i = 0; i < enemies.size();i++)
	{

		enemy = enemies[i];

		for (auto const& projectile : PlayerProjectiles)
		{
			Common::debuginfo.CollisionChecks++;
			if (CheckCollision(*projectile, *enemy))
			{
				float Damage = projectile->DamageDealt;

				enemy->TakeDamage(Damage);
				projectile->Hit();
				if (enemy->GetHealth() <= 0.0f)
				{
					Spawnxp(enemy.get());

				}
			}
		}
		Common::debuginfo.CollisionChecks++;
		
		if (CheckCollisionWithPlayer(*enemy))
		{
			enemy->TakeDamage(2.0f);
			
			player->TakeDamage(1.0f);
		}
		/*
		for (int j = 0; j < enemies.size(); j++)
		{
			Common::debuginfo.CollisionChecks++;
			if (i == j)
				continue;
			if (CheckCollision(*enemies[i], *enemies[j]))
			{
				glm::vec2 test(glm::normalize(enemies[i]->Position - enemies[j]->Position));
				enemy->Position += test;
			}
		}
		*/
	}
	
	for (int i = 0; i < expShards.size();i++)
	{
		auto xp = expShards[i];
		Common::debuginfo.CollisionChecks++;
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
	ImGui::Text("HP : %i", Common::debuginfo.PlayerHealth);
	ImGui::Text("Projectiles : %i", Common::debuginfo.Projectiles);
	ImGui::Text("Enemies : %i", Common::debuginfo.Enemies);
	ImGui::Text("Director credits : %f", this->director->m_credits);
	ImGui::Text("CollisionChecks : %i", Common::debuginfo.CollisionChecks);
	ImGui::Text("Position : %f %f", player->GetPosition().x, player->GetPosition().y);
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
	ImGui::Spacing();
	ImGui::Text("");
	if (ImGui::Button("AddRandomItem"))
	{
		int n = this->cookingMenu.INGREDIENTS.size();
		
		player->inventory.addIngredient(this->cookingMenu.INGREDIENTS.at(randFloat(1, n - 1)), 1);
	}
	ImGui::Spacing();
	ImGui::Text("Ingredients : ");
	for (const auto& [ingredient, quantity] : player->inventory.stock)
	{
		std::string ingredienttxt = ingredient->name + " : " + std::to_string(quantity);
		ImGui::Text(ingredienttxt.c_str());
	}
	ImGui::End();
	ImGui::Begin("Settings tweaker", (bool*)0, flags);
	ImGui::Text("Stats : ");
	ImGui::SliderFloat("Attack speed",&player->stats.AttackSpeed,1.0f, 3.0f);
	ImGui::SliderFloat("Movement speed", &player->stats.PlayerSpeed, 50.0f, 250.0f);
	ImGui::SliderInt("Projectile count", &player->stats.projectileCount, 1, 10);
	ImGui::SliderFloat("Enemy Spawnrate [/s] ", &Common::debuginfo.SpawnRate, 1.0f, 20.0f);
	ImGui::Spacing();
	ImGui::Text("Graphic Settings : ");
	ImGui::SliderInt("PixelSize", &renderer.pixelSize, 1, 64);
	ImGui::SliderInt("LightPixelize", &renderer.LightPixelize, 0, 8);
	ImGui::SliderInt("RenderMode", &renderer.RenderMode, 0, 8);
	ImGui::End();
}

void Game::RenderLevelUp()
{
	/*
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
		*/
}

bool CheckCollision(GameObject& one, GameObject& two)
{
	glm::vec2 onePos = one.GetPosition();
	glm::vec2 twoPos = two.GetPosition();

	glm::vec2 oneSize = one.GetSize();
	glm::vec2 twosize = two.GetSize();

	bool axisX = onePos.x + oneSize.x >= twoPos.x && twoPos.x + twosize.x >= onePos.x;
	bool axisY = onePos.y + oneSize.y >= twoPos.y && twoPos.y + twosize.y >= onePos.y;
	
	return axisX && axisY;
}

bool Game::CheckCollisionWithPlayer(GameObject& one)
{
	GameObject two = *player;

	glm::vec2 onePos = one.GetPosition();
	glm::vec2 twoPos = two.GetPosition();

	glm::vec2 oneSize = one.GetSize();
	glm::vec2 twosize = two.GetSize();

	bool axisX = onePos.x + oneSize.x >= twoPos.x && twoPos.x + twosize.x >= onePos.x;
	bool axisY = onePos.y + oneSize.y >= twoPos.y && twoPos.y + twosize.y >= onePos.y;

	return axisX && axisY;
}

void Spawnxp(GameObject* enemy)
{
	if ((rand() % 2))
	{
		expShards.push_back
		(
			std::make_shared<GameObject>(GameObject(
				enemy->GetPosition(),
				glm::vec2(32.0f, 32.0f),
				0.0f,
				&ResourceManager::GetTexture("tomato"),
				ResourceManager::GetShaderPtr("instancedSprite"),
				PICKUPS

			))
		);
	}
}
//Not used
bool MouseInRange(glm::vec2 MousePos,glm::vec2 start, glm::vec2 end)
{
	return 
		start.x <= MousePos.x &&
		MousePos.x <= end.x && 
		start.y <= MousePos.y &&
		MousePos.y <= end.y;

}