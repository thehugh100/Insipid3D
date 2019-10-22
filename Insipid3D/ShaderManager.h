#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <map>

struct ShaderManager
{
	GLuint getShader(std::string name);
	void flushShaders();

	std::map<std::string, GLuint> shaders;
};
