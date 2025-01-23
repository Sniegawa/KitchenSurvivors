#include "Common.h"

glm::vec2 Common::ScreenSize = glm::vec2(1280,720);

DebugInfo Common::debuginfo = DebugInfo();

float Common::MousePlayerAngle = 0.0f;

std::unordered_map<int, int> Common::lvlmap=
{
	{1,10},{2,15},{3,20},{4,20}
};
std::map<char, Character> Common::Characters;

const std::unordered_map<int, Ingredient> Common::INGREDIENTS =
{
	{-1,{-1,"Empty Item",""}},
	{0,{0,"Bread","pizza"}},
	{1,{1,"Milk","tomato"}},
	{2,{2,"Garlic","tomato"}},
	{3,{3,"Onion","tomato"}},
	{4,{4,"Tomato","tomato"}},
	{5,{5,"Chili","tomato"}},
	{6,{6,"Salt","salt"}},
	{7,{7,"Pepper","pepper"}}
};

const std::unordered_map<int, Recipe> Common::recipes =
{
	{0, {0, INGREDIENTS.at(0), INGREDIENTS.at(1), "Anti-DoT Buff", "Negates all DoT effects"}},
	{1, {1, INGREDIENTS.at(2), INGREDIENTS.at(3), "Poison Aura", "Creates a poison aura"}},
	{2, {2, INGREDIENTS.at(4), INGREDIENTS.at(5), "Fire Boost", "Adds fire DoT to attacks"}}
};

void Common::AddCharacter(char _c, Character _char)
{
	Characters.insert(std::pair<char, Character>(_c, _char));
}

