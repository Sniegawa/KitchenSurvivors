#pragma once
#include "GameObject.h"
#include "Projectile.h"
#include "../Common.h"
#include "../Systems/CookingMenu/CookingMenu.h"
#include<memory>

struct Inventory
{
	std::unordered_map<const Ingredient*, int> stock;

	//Need this variable as trigger for rebuilding Cooking Menu graphic
	bool ChangedState = false;

	void addIngredient(const Ingredient& ingredient, int quantity)
	{
		int stockBefore = this->inventorySize();
		stock[&ingredient] += quantity;

		int stockAfter = this->inventorySize();
		ChangedState = stockAfter>stockBefore;
	} 

	void consumeIngredient(const Ingredient& ingredient, int quantity)
	{
		int stockBefore = this->inventorySize();
		stock[&ingredient] -= quantity;

		int stockAfter = this->inventorySize();
		ChangedState = stockAfter < stockBefore || stock[&ingredient] == 0;
	}

	bool hasIngredient(const Ingredient& ingredient) const
	{
		return stock.find(&ingredient) != stock.end() && stock.at(&ingredient) > 0;
	}

	int inventorySize() const
	{
		int n = 0;
		for (auto kv : stock)
		{
			if (kv.second > 0)
				n++;
		}
		return n;
	}
};

class Weapon;

class Player : public GameObject
{
public:
	Player(glm::vec2 pos, glm::vec2 size, Texture2D* sprite, Shader* shader, RenderLayer layer, std::vector<std::shared_ptr<Projectile>>* pprojptr, glm::vec3 color = glm::vec3(1.0f), float rotation = 0.0f);
	~Player(){}
	void TakeDamage(float amount);
	void UpdateCooldowns(float dt);
	void GetXp(int type);
	void AddEffect(Effect* effect, float Length);

	Weapon* weapons[6];

	PlayerStats stats;
	
	std::vector<Effect*> effects;

	Inventory inventory;

	//Dunno if still neccesary
	int Level = 0;
	float LvlProgress = 0;
	bool Alive = true;
	int Kills;

	float xp;
	float xpToLvl;

	Texture2D* GetNormalMap() { return this->NormalMap; }

	const float& GetHealth() const { return this->Health; }

private:

	float Health = stats.MaxHealth;
	Texture2D* NormalMap;
	float InvulnerabilityCD;
	std::vector<std::shared_ptr<Projectile>>* PlayerProjectilesPtr; //Cursed
};
