#include "Common.h"

glm::vec2 Common::ScreenSize = glm::vec2(1280,720);
std::unordered_map<int, int> Common::lvlmap=
{
	{1,10},{2,15},{3,20},{4,20}
};
std::map<char, Character> Common::Characters;


void Common::AddCharacter(char _c, Character _char)
{
	Characters.insert(std::pair<char, Character>(_c, _char));
}