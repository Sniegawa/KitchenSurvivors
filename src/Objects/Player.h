#pragma once

#include "GameObject.h"
#include <vector>
#include "Projectile.h"
//#include "../Weapons/Weapon.h"
#include "../Common.h"
#include <memory>


struct Inventory
{
	std::unordered_map<const Ingredient*, int> stock;

	void addIngredient(const Ingredient& ingredient, int quantity)
	{
		stock[&ingredient] += quantity;
	} 

	void consumeIngredient(const Ingredient& ingredient, int quantity)
	{
		stock[&ingredient] -= quantity;
	}

	bool hasIngredient(const Ingredient& ingredient) const
	{
		return stock.find(&ingredient) != stock.end() && stock.at(&ingredient) > 0;
	}

};



class Weapon;
class Player : public GameObject
{
public:
	Player(glm::vec2 pos, glm::vec2 size, Texture2D* sprite, Shader* shader, RenderLayer layer, std::vector<std::shared_ptr<Projectile>>* pprojptr, glm::vec3 color = glm::vec3(1.0f), float rotation = 0.0f);
	//glm::vec2* _pos;
	void TakeDamage(float amount);
	void UpdateCooldowns(float dt);
	void GetXp(int type);

	Weapon *weapons[6];
	PlayerStats stats;
	Inventory inventory;


	Texture2D* NormalMap;
	unsigned int Level = 0;
	float LvlProgress = 0;
	float Health = stats.MaxHealth;
	bool Alive = true;
	unsigned int Kills;

	float xp;
	float xpToLvl;
private:

	float InvulnerabilityCD;
	std::vector<std::shared_ptr<Projectile>>* PlayerProjectilesPtr; //Cursed

	
};
