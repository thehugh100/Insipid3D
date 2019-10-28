#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <string>
#include <string.h>

struct Texture
{
	Texture();
	Texture(const char* fileName, bool isLightmap = 0);
	Texture(unsigned char * memData, const unsigned int size, const char* modelName, int texIndex);

	std::string fname;
	std::string modelNameS;
	unsigned char* data;
	int x, y, c;
	GLuint texId;
	int itexIndex;
};

struct TextureManager
{
	std::vector<Texture> textures;
	GLuint loadTexture(std::string fname, bool isLightmap = 0);
	GLuint loadTexture(unsigned char* memData, const unsigned int size, const char* modelName, int texIndex);
	static unsigned int loadCubemap(std::vector<std::string> faces);
};