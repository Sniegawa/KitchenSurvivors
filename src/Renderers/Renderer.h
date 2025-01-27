#pragma once
#include "../ResourceHandlers/Texture.h"
#include "../ResourceHandlers/ShaderLoader.h"
#include "../Objects/GameObject.h"
#include "../Objects/Player.h"
#include "glm.hpp"
#include <unordered_map>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include "../Common.h"

//Renderer class is getting a bit cramped, for futer refactors, some other class would be ideal to store informations about Cooking Menu, and only use Renderer as API to render it's GUI
//for now i dont care XD

struct CookingMenuInformation
{
	GLuint VAO;
	GLuint VBO;
	std::vector<std::vector<glm::vec2>> quads;
	std::vector<glm::vec2> slotCenters;
	//std::unordered_map<int, Ingredient&> Slots;
	std::vector<int> selectedSlots;
};

class Renderer {
public:
	void Render(const std::vector<GameObject*>& gameObjects);
	void RenderLight();
	void RenderPlayer(Player* player);
	void RenderBackground(GameObject* background);
	Renderer();
	void UpdatePlayerPos(glm::vec2 playerPos);
	void UpdateInventoryMenu(const Inventory* inv);
	void RendererSetup();

	void RenderLine(glm::vec2 p1, glm::vec2 p2, glm::vec4 color);
	void RenderSprite(const Texture2D& sprite, glm::vec2 position, float rotation, glm::vec2 scale, glm::vec3 color = glm::vec3(1.0f));
	void RenderCookingMenu(Inventory* inv);



	int pixelSize = 1;
	int LightPixelize = 1;
	int RenderMode = 0;

	glm::vec2* MousePos;

private:
	glm::vec2 PlayerPos;
	void UpdateInstanceData(const std::vector<glm::mat4>& modelMatrices);
	void DrawInstances(GLsizei instanceCount);
	void Innit();
	void InnitPlayerData();
	void InnitBackgroundData();
	void InnitScreenQuad();
	void InnitScreenGBuffer();
	void InnitUIvao();
	void InnitLineVao();
	void RenderLightmap();
	void PrepareLightmap();
	void DownscaleTexture(GLuint inputTex, GLuint outputTex, int factor);

	unsigned int gBuffer = 0;

	unsigned int gPosition = 0;
	unsigned int gNormal = 0;
	unsigned int gAlbedo = 0;
	unsigned int rboDepth = 0;

	int lastPixelSize = 0;

	GLuint VAO = 0;
	GLuint VBO = 0;
	
	GLuint InstanceVBO = 0;
	
	GLuint PlayerVAO = 0;
	GLuint PlayerVBO = 0;
	
	GLuint BackgroundVAO = 0;
	GLuint BackgroundVBO = 0;

	GLuint ScreenQuadVAO = 0;
	GLuint ScreenQuadVBO = 0;

	GLuint CookingMenuVAO = 0;

	GLuint UIspriteVAO = 0;
	GLuint UIspriteVBO = 0;

	GLuint LineVAO = 0;
	GLuint LineVBO = 0;

	GLuint Lightmap = 0;
	GLuint DownscaledLightmap = -1;

	CookingMenuInformation smallestMenuInfo;

	CookingMenuInformation info;

	const float vertices[32] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
};