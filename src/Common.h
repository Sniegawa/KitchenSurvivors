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

struct DebugInfo
{
	int CollisionChecks;
	int Enemies;
	int Projectiles;
	int PlayerHealth;
	int DrawCalls;
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


