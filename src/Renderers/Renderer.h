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
Podzia³ jeszce na shadery,
- W gameobject podzia³ na pointer do shadera i Shader uniform który bezie z jakiejœ powalonej zmiennej std::variant
- W shaderze zmieniæ z wielu funkcji na jedn¹ z overloadem.
- ChatGPT podpowiada zrobienie czegoœ takiego
	void SetUniform(const std::string& name, const UniformValue& value) {
		std::visit([&](auto&& arg) { SetUniformByType(name, arg); }, value);
	}
	ale nie wiem czy nie bêdzie to bez sensownym skomplikowaniem gdybym chcia³ w ³atwy sposób ustawiæ uniforma, jak np do tekstury. (musze pomysleæ)

Uniformy dla ka¿dej instancji: !!!!(SSBO)!!!!
po ogarnieciu tego wy¿ej trzeba ogarnaæ jak wgl zaimplementowaæ przesy³ uniformów dla ka¿dego obiektu, 
Aleto na póxniej najpierw trzeba przepisaæ ustawianie uniformów w klasie shader

*/


class Renderer {
public:
	void Render(const std::vector<std::shared_ptr<GameObject>>& gameObjects);
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


	GLuint VAO = 0, VBO = 0, InstanceVBO = 0, PlayerVAO = 0, PlayerVBO = 0, BackgroundVAO = 0, BackgroundVBO = 0;
};