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


/*

TODO:
Podzia� jeszce na shadery,
- W gameobject podzia� na pointer do shadera i Shader uniform kt�ry bezie z jakiej� powalonej zmiennej std::variant
- W shaderze zmieni� z wielu funkcji na jedn� z overloadem.
- ChatGPT podpowiada zrobienie czego� takiego
	void SetUniform(const std::string& name, const UniformValue& value) {
		std::visit([&](auto&& arg) { SetUniformByType(name, arg); }, value);
	}
	ale nie wiem czy nie b�dzie to bez sensownym skomplikowaniem gdybym chcia� w �atwy spos�b ustawi� uniforma, jak np do tekstury. (musze pomysle�)

Uniformy dla ka�dej instancji: !!!!(SSBO)!!!!
po ogarnieciu tego wy�ej trzeba ogarna� jak wgl zaimplementowa� przesy� uniform�w dla ka�dego obiektu, 
Aleto na p�xniej najpierw trzeba przepisa� ustawianie uniform�w w klasie shader

*/


class Renderer {
public:
	void Render(const std::vector<GameObject*>& gameObjects);
	void RenderLight();
	void RenderPlayer(Player* player);
	void RenderBackground(GameObject* background);
	Renderer();
	void UpdatePlayerPos(glm::vec2 playerPos);
	void RendererSetup();
private:
	glm::vec2 PlayerPos;
	void UpdateInstanceData(const std::vector<glm::mat4>& modelMatrices);
	void DrawInstances(GLsizei instanceCount);
	void Innit();
	void InnitPlayerData();
	void InnitBackgroundData();
	void InnitScreenQuad();
	void InnitScreenBuffers();

	unsigned int gBuffer;

	unsigned int gPosition;
	unsigned int gNormal;
	unsigned int gAlbedo;
	unsigned int rboDepth;


	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint InstanceVBO = 0;
	GLuint PlayerVAO = 0;
	GLuint PlayerVBO = 0;
	GLuint BackgroundVAO = 0;
	GLuint BackgroundVBO = 0;
	GLuint ScreenQuadVAO = 0;
	GLuint ScreenQuadVBO = 0;
};