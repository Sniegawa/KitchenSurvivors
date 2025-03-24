#pragma once

#include "glm.hpp"

class Player;

class Camera
{
public:
	glm::vec2 m_playerFollower;

	
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	void Update(float dt);
	void Setup(Player* player);

private:
	Player* m_playerPtr;
	const float m_followingScale = 150.0f;
	const float m_maxDist = 250.0f;

	glm::vec2 offset;

	const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

};