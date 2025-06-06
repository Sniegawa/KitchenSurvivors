#include "Camera.h"

#include "../Objects/Player.h"
#include "../ResourceHandlers/ResourceManager.h"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

void Camera::Update(float dt)
{
	//For now i haven't decided if i should use smooth following but i'll leave it here implemented poorly
	/*
	float dist = glm::distance(this->m_playerFollower, this->m_playerPtr->Position - offset);
	if (dist <= 1.0f)
		return;
	glm::vec2 dir = glm::normalize((this->m_playerPtr->Position - offset) - this->m_playerFollower);
	
	this->m_playerFollower += dir * dt * m_followingScale * ((dist >= this->m_maxDist) ? 10.0f : 1.0f);

	*/
	glm::vec3 cameraPos = glm::vec3(this->m_playerPtr->GetPosition() - offset, 0.0f);//glm::vec3(this->m_playerFollower, 0.0f);

	viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	ResourceManager::GetShader("instancedSprite").SetUniform("view", viewMatrix, true);
	ResourceManager::GetShader("sprite").SetUniform("view", viewMatrix, true);
}

void Camera::Setup(Player* player)
{
	this->m_playerPtr = player;
	this->offset = (Common::ScreenSize / 2.0f) - (glm::vec2(player->GetSprite()->Width, player->GetSprite()->Height) * 0.75f);
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Common::ScreenSize.x), static_cast<float>(Common::ScreenSize.y), 0.0f, -1.0f, 10.0f);

	ResourceManager::GetShader("instancedSprite").SetUniform("projection", projection, true);

	ResourceManager::GetShader("sprite").SetUniform("projection", projection, true);

	ResourceManager::GetShader("text").SetUniform("projection", projection, true);

	ResourceManager::GetShader("UI").SetUniform("projection", projection, true);

	ResourceManager::GetShader("CookingMenu").SetUniform("projection", projection, true);

	ResourceManager::GetShader("Line").SetUniform("screenSize", Common::ScreenSize, true);

	this->m_playerFollower = this->m_playerPtr->GetPosition();

	glm::vec3 cameraPos = glm::vec3(this->m_playerFollower, 0.0f);

	viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	ResourceManager::GetShader("instancedSprite").SetUniform("view", viewMatrix, true);
	ResourceManager::GetShader("sprite").SetUniform("view", viewMatrix, true);
}