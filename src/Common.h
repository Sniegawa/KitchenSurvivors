#pragma once
#include <unordered_map>
#include <glm.hpp>
#include <map>
struct PlayerStats {
	float AttackSpeed = 1.0f;
	int MaxHealth = 100;
	int projectileCount = 1;
	float PlayerSpeed = 75.0f;
	float ExpirienceMultiplier = 1.0f;
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
	pointLight(glm::vec2 _coords, glm::vec3 _color) : coords(glm::vec4(_coords, 0.0f, 0.0f)), color(glm::vec4(_color, 0.0f)) {};
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

	static std::unordered_map<int, int> lvlmap;

	static std::map<char, Character> Characters;

	static void AddCharacter(char _c, Character _char);
private:
	Common() { this->ScreenSize = glm::vec2(1280, 720); this->debuginfo = DebugInfo(); };
};


