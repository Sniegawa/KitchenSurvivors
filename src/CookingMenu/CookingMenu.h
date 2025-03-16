#pragma once
#include "Effects/Effect.h"
#include "Effects/EffectsInclude.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <unordered_map>
#include <memory>
#include <vector>

class Texture2D;

struct CookingMenuInformations
{
	GLuint VAO;
	GLuint VBO;
	std::vector<std::vector<glm::vec2>> quads;
	std::vector<glm::vec2> slotCenters;
	//std::unordered_map<int, Ingredient&> Slots;
	std::vector<int> selectedSlots;
};

struct Ingredient
{
	int id;
	std::string name;
	std::string description;
	const Texture2D& sprite;

	Ingredient(int _id, std::string _name, const Texture2D& _sprite) : id(_id), name(_name), sprite(_sprite), description(std::string("")) {}
};

struct Recipe
{
	int id;
	int ingredient1ID;
	int ingredient2ID;

	std::string name;
	int effectID;

	Recipe(int _id, int _ingredient1, int _ingredient2, std::string _name, int _effect) :
		id(_id),
		ingredient1ID(_ingredient1),
		ingredient2ID(_ingredient2),
		name(_name),
		effectID(_effect)
	{
	}
	
};




class CookingMenu
{	
public:

	std::unordered_map<int, Ingredient> INGREDIENTS;

	std::unordered_map<int, Recipe> RECIPES;

	std::unordered_map<int, Effect*> EFFECTS;

	CookingMenu() {};

	void InnitCookingMenu(Player* _player_ptr);
	void UpdateCookingMenu();

	const CookingMenuInformations& GetCookingMenuInfo() const { return this->m_currentMenuInfo; }
	const CookingMenuInformations& GetSmallestCookingMenu() const { return this->m_smallestMenuInfo; }

private:
	void InnitIngredients();
	void InnitRecipes();
	void InnitEffects();
	Player* m_player_ptr;
	CookingMenuInformations m_smallestMenuInfo;
	CookingMenuInformations m_currentMenuInfo;
};