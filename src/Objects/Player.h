#pragma once
#include "GameObject.h"
#include "Projectile.h"
#include "../Common.h"
#include "../CookingMenu/CookingMenu.h"
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
		ChangedState = stockAfter < stockBefore;
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
	//glm::vec2* _pos;
	void TakeDamage(float amount);
	void UpdateCooldowns(float dt);
	void GetXp(int type);
	void AddEffect(Effect* effect, float Length);

	Weapon* weapons[6];
	PlayerStats stats;
	
	std::vector<Effect*> effects;

	Inventory inventory;

	Texture2D* NormalMap;
	int Level = 0;
	float LvlProgress = 0;
	float Health = stats.MaxHealth;
	bool Alive = true;
	int Kills;

	float xp;
	float xpToLvl;
private:

	float InvulnerabilityCD;
	std::vector<std::shared_ptr<Projectile>>* PlayerProjectilesPtr; //Cursed
};
