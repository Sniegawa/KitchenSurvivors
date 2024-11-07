#pragma once
#include "../ResourceHandlers/Texture.h"
#include "../ResourceHandlers/ShaderLoader.h"
#include "../Objects/GameObject.h"
#include "glm.hpp"
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <iostream>

class Renderer {
public:
	void Render(const std::vector<std::shared_ptr<GameObject>>& gameObjects);
	void Innit();
	Renderer() {};
	void UpdatePlayerPos(glm::vec2 playerPos);
private:
	glm::vec2 PlayerPos;
	void UpdateInstanceData(const std::vector<glm::mat4>& modelMatrices);
	void DrawInstances(GLsizei instanceCount);
	GLuint VAO = 0, VBO = 0, InstanceVBO = 0;
};