#pragma once
#include <unordered_map>
#include <glm.hpp>
#include <map>
#include <string>

#include "../src/ResourceHandlers/ResourceManager.h"

#define LOG_WARNING(msg)\
{\
	std::printf("WARNING : ", msg);\
}

struct PlayerStats 
{
	float AttackSpeed = 1.0f;
	int MaxHealth = 100;
	int projectileCount = 1;
	float PlayerSpeed = 75.0f;
	float ExpirienceMultiplier = 1.0f;
};




//LearonOpenGL.com tutorial
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
	int CollisionChecks = 0;
	int Enemies = 0;
	int Projectiles = 0;
	int PlayerHealth = 0;
	int DrawCalls = 0;
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

	static void AddCharacter(char _c, Character _char);
private:
	Common() { this->ScreenSize = glm::vec2(1280, 720); this->debuginfo = DebugInfo(); };
};