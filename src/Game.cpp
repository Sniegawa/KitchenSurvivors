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

#include "Renderers/Renderer.h"
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

SpriteRenderer* drenderer;

PlayerRenderer* playerRenderer;

EnemyRenderer* enemyRenderer;

TextRenderer* textRenderer;

SpriteRenderer* UIRenderer;

extern float MousePlayerAngle;

Player* player;


struct pointLight
{
	glm::vec2 coords;// vec2 pos, float radius, float ??
	glm::vec3 color;
	pointLight(glm::vec2 _coords , glm::vec3 _color) : coords(_coords),color(_color)  {};
};

std::vector<pointLight> lights;


std::vector<std::shared_ptr<Enemy>> enemies;

extern glm::vec2 PlayerPosition = glm::vec2(0);

extern std::vector<std::shared_ptr<Projectile>> PlayerProjectiles;

std::vector<std::shared_ptr<GameObject>> expShards;

Game::~Game()
{
	delete drenderer;
	delete UIRenderer;
	delete playerRenderer;
	delete player;
}

ImGuiWindowFlags flags = 0;

int lastlvl;

void Game::Init()
{
	this->renderer = Renderer();
	this->renderer.Innit();
	//load shaders
	ResourceManager::LoadShader("src/Shaders/SpriteShader.vert", "src/Shaders/SpriteShader.frag", "sprite");
	ResourceManager::LoadShader("src/Shaders/InstancedSpriteShader.vert", "src/Shaders/InstancedSpriteShader.frag", "instancedSprite");
	ResourceManager::LoadShader("src/Shaders/TextShader.vert", "src/Shaders/TextShader.frag", "text");
	ResourceManager::LoadShader("src/Shaders/UIShader.vert", "src/Shaders/UIShader.frag","UI");
	ResourceManager::LoadShader("src/Shaders/DefferedLight.vert", "src/Shaders/DefferedLight.frag", "light");
	ScreenCenter = glm::vec2(this->Width / 2, this->Height / 2);

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	
	ResourceManager::GetShader("instancedSprite").Use();
	ResourceManager::GetShader("instancedSprite").SetMatrix4("projection", projection);

	ResourceManager::GetShader("sprite").Use();
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	//ResourceManager::GetShader("sprite").SetInteger("image", 0);

	ResourceManager::GetShader("text").Use();
	ResourceManager::GetShader("text").SetMatrix4("projection", projection);

	ResourceManager::GetShader("UI").Use();
	ResourceManager::GetShader("UI").SetMatrix4("projection", projection);

	ResourceManager::LoadTexture("src/Textures/512X512.png", false, "background");
	ResourceManager::LoadTexture("src/Textures/pizza.png", true, "pizza");
	ResourceManager::LoadTexture("src/Textures/player.png", true, "player");
	ResourceManager::LoadTexture("src/Textures/knife.png", true, "knife");
	ResourceManager::LoadTexture("src/Textures/WIDELEC.png", true, "fork");
	ResourceManager::LoadTexture("src/Textures/TASAK.png", true, "butcher");
	ResourceManager::LoadTexture("src/Textures/Tomato.png", true, "tomato");

	ResourceManager::LoadTexture("src/Textures/lvlup.png", true, "lvluphud");

	drenderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	UIRenderer = new SpriteRenderer(ResourceManager::GetShader("UI"));
	playerRenderer = new PlayerRenderer(SpriteRenderer(ResourceManager::GetShader("sprite")));
	enemyRenderer = new EnemyRenderer(SpriteRenderer(ResourceManager::GetShader("sprite")), &PlayerPosition);
	textRenderer = new TextRenderer(ResourceManager::GetShader("text"));
	
	Texture2D PlayerSprite = ResourceManager::GetTexture("pizza");
	
	glm::vec2 StartingPlayerPosition = glm::vec2(static_cast<float>(this->Width) / 2 - PlayerSprite.Width, static_cast<float>(this->Height) / 2 - PlayerSprite.Height) + glm::vec2(30.0f,16.0f);

	player = new Player(StartingPlayerPosition, glm::vec2(PlayerSprite.Width, PlayerSprite.Height)*1.5f, &ResourceManager::GetTexture("pizza"),&PlayerProjectiles,&PlayerPosition);

	flags |= ImGuiWindowFlags_AlwaysAutoResize;
	flags |= ImGuiWindowFlags_NoCollapse;

	player->weapons[0] = new ThrownWeapon("fork","Fork","Throw a fork at enemy", &player->stats, &PlayerPosition, 1.0f);
	player->weapons[1] = new OrbitWeapon("knife", "Orbit", &player->stats, &PlayerPosition,5.0f);
	lastlvl = player->Level;

	srand(time(0));
	for (int i = 0; i < 25; i++)
	{
		glm::vec2 pos;
		glm::vec3 col;
		pos = glm::vec2(randFloat(0,2000), randFloat(0,2000));
		col = randColor();
		lights.emplace_back(pos, col);
	}

	//ResourceManager::GetShader("light").Use();

	//unsigned int SSBOLights;
	//glGenBuffers(1, &SSBOLights);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOLights);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(pointLight) * lights.size() , &lights[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, SSBOLights, 0, sizeof(pointLight) * lights.size());


	//glBindBufferBase(ResourceManager::GetShader("light").ID,1 , SSBOLights);

}

//---------------------
//| LIGHT RENDER PASS |
//---------------------

void Game::RenderLight()
{
	ResourceManager::GetShader("light").Use();
	float quadVertices[] = {
		// positions   // texture coords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}


//------------------------
//| GEOMETRY RENDER PASS |
//------------------------

void Game::Render()
{
	Common::debuginfo.DrawCalls = 0;



	//ResourceManager::GetShader("sprite").SetVector2f("PlayerPos", PlayerPosition);
	//TEMP BACKGROUND
	drenderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f)-PlayerPosition, glm::vec2(this->Width*10.0f, this->Height*10.0f), 0.0f);
	Common::debuginfo.DrawCalls++;
	
	std::vector<std::shared_ptr<GameObject>> RenderData;
	//Data at top is rendered first
	RenderData.insert(RenderData.end(), expShards.begin(), expShards.end());
	RenderData.insert(RenderData.end(), enemies.begin(), enemies.end());
	RenderData.insert(RenderData.end(), PlayerProjectiles.begin(), PlayerProjectiles.end());

	this->renderer.Render(RenderData);

	//FINALLY DRAW PLAYER
	player->Draw(*playerRenderer);
	Common::debuginfo.DrawCalls++;



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
				&ResourceManager::GetTexture("pizza"),
				player
			)));
		spawnerTime = 0.0f;
		
	}


	for (auto enemy : enemies)
	{
		enemy->Update(dt);
	}
	for (int i = 0; i < enemies.size(); i++)
	{
		if (enemies[i]->isDead)
		{

			enemies.erase(enemies.begin() + i);
			continue;
		}
	}

	player->UpdateCooldowns(dt);



	renderer.UpdatePlayerPos(PlayerPosition);

	Common::debuginfo.Enemies = enemies.size();
	Common::debuginfo.Projectiles = PlayerProjectiles.size();
	Common::debuginfo.PlayerHealth = player->Health;
	
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
	Common::debuginfo.CollisionChecks = 0;
	for (int i = 0; i < enemies.size();i++)
	{
		auto enemy = enemies[i];

		for (auto const& projectile : PlayerProjectiles)
		{
			Common::debuginfo.CollisionChecks++;
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
		Common::debuginfo.CollisionChecks++;
		
		if (CheckCollisionWithPlayer(*enemy))
		{
			enemy->TakeDamage(2.0f);
			player->TakeDamage(1.0f);
		}
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
	ImGui::Text("CollisionChecks : %i", Common::debuginfo.CollisionChecks);
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
				0.0f,
				&ResourceManager::GetTexture("tomato")

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