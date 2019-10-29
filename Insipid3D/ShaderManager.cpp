#include "ShaderManager.h"
#include "GLShader.h"
#include <filesystem>

GLuint ShaderManager::getShader(std::string name)
{
	if (shaders.find(name) == shaders.end())
	{
		std::string geom = std::string(name + "_g.glsl").c_str();
		std::string geomParam = "";
		if (std::filesystem::exists(geom))
			geomParam = geom;

		shaders[name] = LoadShader(std::string(name + "_v.glsl").c_str(), std::string(name + "_f.glsl").c_str(), geomParam.c_str());
		return shaders[name];
	}

	return shaders[name];
}

void ShaderManager::flushShaders()
{
	shaders.clear();
}
