
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "resource_manager.h"
#include "stb_image.h"

//静态变量初始化
std::map<std::string, Texture2D> ResourceManager::Textures;
std::map<std::string, Shader> ResourceManager::Shaders;

Shader ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name)
{
	Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char* file, bool alpha, std::string name)
{
	Textures[name] = loadTextureFromFile(file, alpha);
	return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
	return Textures[name];
}

void ResourceManager::Clear()
{
	for (auto iter: Shaders) {
		glDeleteProgram(iter.second.ID);
	}
	for (auto iter : Textures) {
		glDeleteTextures(1, &iter.second.ID);
	}
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
	std::cout << std::filesystem::current_path() << std::endl;
	
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try
	{
		//open files
		std::ifstream vertexShaderFile;
		vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		vertexShaderFile.open(vShaderFile);
		
		std::ifstream fragmentShaderFile;
		fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragmentShaderFile.open(fShaderFile);

		std::stringstream vShaderStream, fShaderStram;

		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStram << fragmentShaderFile.rdbuf();
		vertexShaderFile.close();
		fragmentShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStram.str();
		if (gShaderFile != nullptr) {
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files: " << e.what() <<std::endl;
	}

	Shader shader;
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* gShaderCode = geometryCode.c_str();
	shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
{
	Texture2D texture;
	if (alpha) {
		//default GL_RGB
		texture.Internal_Format = GL_RGBA;
		texture.Image_Format = GL_RGBA;
	}
	int width, height, nrChannels;
	unsigned char * data = stbi_load(file, &width, &height, &nrChannels, 0);
	texture.Generate(width, height, data);
	stbi_image_free(data);
	return texture;
}
