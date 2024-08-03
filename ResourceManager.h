#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include<map>
#include <string>

#include <GL/glew.h>

#include"Texture.h"
#include"ShaderLoader.h"

class ResourceManager
{
public:
	static std::map < std::string, Shader> Shaders;
	static std::map <std::string, Texture2D>Textures;

	static Shader LoadShader(const char* vertexShaderFile, const char* fragmentShaderFile, std::string name);

	static Shader& GetShader(std::string name);

	static Texture2D LoadTexture(const char* file, bool alpha, std::string name);

	static Texture2D& GetTexture(std::string name);

	static void Clear();

private:
	ResourceManager() {}

	static Shader loadShaderFromFile(const char* vShaderFile, const char* fShaderFile);

	static Texture2D loadTextureFromFile(const char* file, bool alpha);
};

#endif // !RESOURCE_MANAGER_H
