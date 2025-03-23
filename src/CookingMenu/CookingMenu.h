#pragma once
#include "Effects/Effect.h"
#include "Effects/EffectsInclude.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <unordered_map>
#include <memory>
#include <vector>
#include <set>

class Texture2D;

struct Slot
{
	int slotID;
	int ingredientID;
	Slot(int _id, int _ing) : slotID(_id) , ingredientID(_ing) {};
	Slot() {}
};

struct CookingMenuInformations
{
	GLuint VAO = 0;
	GLuint VBO = 0;
	std::vector<std::vector<glm::vec2>> quads;
	std::vector<glm::vec2> slotCenters;
};

struct Ingredient
{
	int id;
	std::string name;
	std::string description;
	Texture2D* sprite;

	Ingredient(int _id, std::string _name, Texture2D* _sprite);
	Ingredient();
};

struct Recipe
{
	int id;
	std::vector<uint8_t> ingredients;

	std::string name;
	int effectID;

	Recipe(int _id, std::vector<uint8_t> _ingredients, std::string _name, int _effect) :
		id(_id),
		ingredients(_ingredients),
		name(_name),
		effectID(_effect)
	{
	}

	bool matches(const std::vector<int>& input) const
	{
		return std::multiset<int>(ingredients.begin(), ingredients.end()) == std::multiset<int>(input.begin(), input.end());
	}
	
};

class CookingMenu
{	
public:

	std::unordered_map<int, Ingredient> INGREDIENTS;

	std::unordered_map<int, Recipe> RECIPES;

	std::unordered_map<int, Effect*> EFFECTS;

	CookingMenu() {};

	Slot HoveredSlot;
	std::vector<Slot> selectedSlots;

	void InnitCookingMenu(Player* _player_ptr);
	void UpdateCookingMenu();
	void ResetSlots();
	void SelectHoveredSlot();
	void Cook();
	CookingMenuInformations& GetCookingMenuInfo() { return this->m_currentMenuInfo; }
	CookingMenuInformations& GetSmallestCookingMenu() { return this->m_smallestMenuInfo; }

private:
	void InnitIngredients();
	void InnitRecipes();
	void InnitEffects();
	Player* m_player_ptr = nullptr;
	CookingMenuInformations m_smallestMenuInfo;
	CookingMenuInformations m_currentMenuInfo;
};