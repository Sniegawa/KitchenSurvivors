#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "GameObject.h"

class Player : public GameObject
{
public:
	Player(const GameObject& g) : GameObject(g), Level(0),LvlProgress(0),Kills(0){}

	unsigned int Level = 0;
	float LvlProgress = 0;
	
	//Ekwipunek broni i pasywek

private:
	unsigned int Kills;

};


#endif // !PLAYER_HPP
