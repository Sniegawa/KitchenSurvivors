#pragma once
#include "../Common.h"
#include "../ResourceHandlers/ResourceManager.h"
#include <string>
#include <glm.hpp>
class TextRenderer
{
public:
	TextRenderer() {};
	TextRenderer(Shader& _shader);

	void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);
private:
	Shader shader;
	unsigned int quadVAO;
};