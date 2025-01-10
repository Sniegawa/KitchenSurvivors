#pragma once
#include <unordered_map>
#include <glm.hpp>
#include <map>
#include <string>

struct PlayerStats {
	float AttackSpeed = 1.0f;
	int MaxHealth = 100;
	int projectileCount = 1;
	float PlayerSpeed = 75.0f;
	float ExpirienceMultiplier = 1.0f;
};



struct Ingredient
{
	int id;
	std::string name;
	std::string description;

	Ingredient(int _id, std::string _name) : id(_id), name(_name), description(std::string("")) {}
};


struct Recipe
{
	int id;
	const Ingredient& ingredient1;
	const Ingredient& ingredient2;

	std::string name;
	std::string effect;

	Recipe(int _id,const Ingredient& _ingredient1,const Ingredient& _ingredient2, std::string _name, std::string _effect) :
		id(_id),
		ingredient1(_ingredient1),
		ingredient2(_ingredient2),
		name(_name),
		effect(_effect)
	{}


};

//LearonOpenGL.com tutorials
struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

struct pointLight
{
	glm::vec4 coords;// vec2 pos, float radius, float ??
	glm::vec4 color;
	pointLight(glm::vec4 _coords, glm::vec4 _color) : coords(_coords), color(_color) {};
};

struct DebugInfo
{
	int CollisionChecks;
	int Enemies;
	int Projectiles;
	int PlayerHealth;
	int DrawCalls;
	float SpawnRate = 1.0f;
};

class Common
{
public:
	static DebugInfo debuginfo;
	static glm::vec2 ScreenSize;
	static float MousePlayerAngle;

	static std::unordered_map<int, int> lvlmap;

	static std::map<char, Character> Characters;

	static const std::unordered_map<int, Ingredient> INGREDIENTS;

	static const std::unordered_map<int, Recipe> recipes;

	static void AddCharacter(char _c, Character _char);
private:
	Common() { this->ScreenSize = glm::vec2(1280, 720); this->debuginfo = DebugInfo(); };
};


