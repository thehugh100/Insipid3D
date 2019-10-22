#include "ShaderManager.h"
#include "GLShader.h"

GLuint ShaderManager::getShader(std::string name)
{
	if (shaders.find(name) == shaders.end())
	{
		shaders[name] = LoadShader(std::string(name + "_v.glsl").c_str(), std::string(name + "_f.glsl").c_str());
		return shaders[name];
	}

	return shaders[name];
}

void ShaderManager::flushShaders()
{
	shaders.clear();
}
