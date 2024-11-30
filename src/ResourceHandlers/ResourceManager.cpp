#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION    
#include "stb_image.h"

std::map<std::string, Texture2D> ResourceManager::Textures;
std::map<std::string, Shader>  ResourceManager::Shaders;
std::map<std::string, ComputeShader> ResourceManager::ComputeShaders;

Shader ResourceManager::LoadShader(const char* vertexShaderFile, const char* fragmentShaderFile, std::string name)
{
	std::cout << "\n" << "Compiling shader " << name << std::endl;
	Shaders[name] = loadShaderFromFile(vertexShaderFile, fragmentShaderFile);
	return Shaders[name];
}

Shader& ResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}

Shader* ResourceManager::GetShaderPtr(std::string name)
{
	return &Shaders[name];
}

ComputeShader ResourceManager::LoadComputeShader(const char* computeFile, std::string name)
{
	std::cout << "\n" << "Compiling compute shader" << name << std::endl;
	ComputeShaders[name] = loadComputeShaderFromFile(computeFile);
	return ComputeShaders[name];
}

ComputeShader& ResourceManager::GetComputeShader(std::string name)
{
	return ComputeShaders[name];
}

Texture2D ResourceManager::LoadTexture(const char* file, bool alpha, std::string name)
{
	std::cout << "\n" << "Texture " << name << " loaded succesfully" << std::endl;
	Textures[name] = loadTextureFromFile(file, alpha);
	return Textures[name];
}

Texture2D& ResourceManager::GetTexture(std::string name)
{
	return Textures[name];
}

void ResourceManager::Clear()
{
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile)
{
	std::string vertexCode;
	std::string fragmentCode;

	try 
	{
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream, fShaderStream;
		//read file buffer
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		//close files
		vertexShaderFile.close();
		fragmentShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();


	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files " << std::endl;
	}
	const char *vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	Shader shader;
	shader.Compile(vShaderCode, fShaderCode);
	return shader;

}

ComputeShader ResourceManager::loadComputeShaderFromFile(const char* computeFile)
{
	std::string ComputeCode;

	try
	{
		std::ifstream computeShaderFile(computeFile);
		std::stringstream cShaderStream;

		cShaderStream << computeShaderFile.rdbuf();
		computeShaderFile.close();

		ComputeCode = cShaderStream.str();
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read compute shader file" << std::endl;
	}
	const char* cShaderCode = ComputeCode.c_str();
	ComputeShader shader;
	shader.Compile(cShaderCode);
	return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
{
	Texture2D texture;
	if (alpha)
	{
		texture.Internal_Format = GL_RGBA;
		texture.Image_Format = GL_RGBA;
	}

	int width, height, nrChannels;
	unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
	texture.Generate(width, height, data);
	
	stbi_image_free(data);
	
	return texture;
}