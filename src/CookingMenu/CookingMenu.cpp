#include "CookingMenu.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>

#include "../ResourceHandlers/Texture.h"
#include "../Objects/Player.h"

CookingMenuInformations createCircleInformation(float innerRadius, float outerRadius, int edges)
{
	CookingMenuInformations inf;

	std::vector<std::vector<glm::vec2>> quads;

	float angleStep = 2.0f * glm::pi<float>() / edges;

	for (int i = 0; i < edges; ++i)
	{
		float angle1 = i * angleStep;
		float angle2 = (i + 1) * angleStep;

		glm::vec2 inner1(innerRadius * cos(angle1), innerRadius * sin(angle1));
		glm::vec2 inner2(innerRadius * cos(angle2), innerRadius * sin(angle2));
		glm::vec2 outer1(outerRadius * cos(angle1), outerRadius * sin(angle1));
		glm::vec2 outer2(outerRadius * cos(angle2), outerRadius * sin(angle2));

		quads.push_back({ inner1,outer1,outer2,inner2 });
		glm::vec2 center = (inner1 + outer1 + outer2 + inner2) / 4.0f;



		inf.slotCenters.push_back(center);
	}

	inf.quads = quads;

	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Allocate buffer size (enough for all quads)
	glBufferData(GL_ARRAY_BUFFER, edges * 4 * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);

	// Set vertex attribute pointers
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	inf.VAO = VAO;
	inf.VBO = VBO;
	return inf;
}

void CookingMenu::InnitCookingMenu(Player* _player_ptr)
{
	this->m_player_ptr = _player_ptr;

	this->InnitIngredients();
	this->InnitRecipes();
	this->InnitEffects();
	this->UpdateCookingMenu();
	this->m_smallestMenuInfo = createCircleInformation(125, 260, 5);
}


void CookingMenu::InnitIngredients()
{
	this->INGREDIENTS =
	{
		//{-1,{-1,"Empty Item",ResourceManager::GetTexture("tomato")}},
		{0,{0,"Bread",ResourceManager::GetTexture("pizza")}},
		{1,{1,"Milk",ResourceManager::GetTexture("tomato")}},
		{2,{2,"Garlic",ResourceManager::GetTexture("tomato")}},
		{3,{3,"Onion",ResourceManager::GetTexture("tomato")}},
		{4,{4,"Tomato",ResourceManager::GetTexture("tomato")}},
		{5,{5,"Chili",ResourceManager::GetTexture("tomato")}},
		{6,{6,"Salt",ResourceManager::GetTexture("salt")}},
		{7,{7,"Pepper",ResourceManager::GetTexture("pepper")}}
	};
}

void CookingMenu::InnitRecipes()
{
	this->RECIPES =
	{
		//ID -> Recipe  {id,ingr1ID,ingr2ID Name			EffectID
		{0,				{0,	6,		7,		"Double trouble", 0}},
		{1,			    {1, 2,		3,		"Poison Aura", 1}},
		{2,				{2, 4,		5,		"Fire Boost", 2}}

	};
}

void CookingMenu::InnitEffects()
{
	this->EFFECTS =
	{
		{0,new E_DoubleTrouble(0,"Double trouble", "Prepare for double and make it trouble!",ResourceManager::GetTexture("knife") , this->m_player_ptr)},
		{1,new E_DoubleTrouble(1,"Double trouble", "Prepare for double and make it trouble!",ResourceManager::GetTexture("knife") , this->m_player_ptr)}
	};
}

void CookingMenu::UpdateCookingMenu()
{
	this->m_currentMenuInfo = createCircleInformation(125, 260, this->m_player_ptr->inventory.inventorySize());
}