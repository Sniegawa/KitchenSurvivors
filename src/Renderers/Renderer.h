#pragma once

#include "../Objects/Player.h"
#include "../Common.h"
#include "../ResourceHandlers/ResourceManager.h"

class Renderer {
public:
	void Render(const std::vector<GameObject*>& gameObjects);
	void RenderLight();
	void RenderPlayer(Player* player);
	void RenderBackground(GameObject* background);
	Renderer();
	void UpdatePlayerPos(glm::vec2 playerPos);
	void RendererSetup();

	void RenderLine(glm::vec2 p1, glm::vec2 p2, glm::vec4 color);
	void RenderSprite(const Texture2D& sprite, glm::vec2 position, float rotation, glm::vec2 scale, glm::vec3 color = glm::vec3(1.0f));
	
	void RenderCookingMenu(Inventory* inv, CookingMenu& cookingMenu);

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

	GLuint UIspriteVAO = 0;
	GLuint UIspriteVBO = 0;

	GLuint LineVAO = 0;
	GLuint LineVBO = 0;

	GLuint Lightmap = 0;
	GLuint DownscaledLightmap = -1;

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