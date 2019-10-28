#include "GL/glew.h"
#include "TextureManager.h"
#include <stb_image.h>
#include <ostream>
#include <sstream>

Texture::Texture()
{
	fname = "";
	data = nullptr;
	x = y = c = 0;
	texId = 0;
	modelNameS = "";
	itexIndex = -1;
}

Texture::Texture(const char* fileName, bool isLightmap)
{
	itexIndex = -1;
	modelNameS = "";
	fname = fileName;
	data = stbi_load(fileName, &x, &y, &c, 0);

	if (data)
	{
		GLenum format = GL_RGB;
		if (c == 4)
			format = GL_RGBA;

		if (c == 1)
			format = GL_R;

		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, format, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
		std::cout << "Loaded Texture '" << fileName << "' (" << x << "x" << y << ") " << c << " components. ID: " << texId << std::endl;

		
		if (!isLightmap)
		{
			glGenerateTextureMipmap(texId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);
		}
		else
		{
			glGenerateTextureMipmap(texId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}
	else
	{
		std::cout << "Failed to load " << fileName << std::endl;
		return;
	}
}

Texture::Texture(unsigned char* memData, const unsigned int size, const char* modelName, int texIndex)
{
	itexIndex = texIndex;
	modelNameS = modelName;
	std::ostringstream tName;
	tName << modelName << "_internal_texture_" << texIndex;
	fname = tName.str();

	data = stbi_load_from_memory(memData, size, &x, &y, &c, 0);

	if (x % 2 != 0 || y % 2 != 0)
	{
		std::cout << modelName << ": skipping " << texIndex << ", width or height not a multiple of 2... " << x << ", " << y << std::endl;
		//return;
	}

	if (!data)
	{
		std::cout << "Failed to load " << tName.str() << std::endl;
		return;
	}
	GLenum format = GL_RGB;
	if (c == 4)
		format = GL_RGBA;

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, format, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	std::cout << "Loaded Texture '" << fname << "' (" << x << "x" << y << ") " << c << " components. ID: " << texId << std::endl;

	glGenerateTextureMipmap(texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);
}

GLuint TextureManager::loadTexture(std::string fname, bool isLightmap)
{
	for (auto& i : textures)
	{
		if (i.fname == fname)
		{
			std::cout << "Loading " << fname << ": texture already loaded, skipping." << std::endl;
			return i.texId;
		}
	}
	Texture t = Texture(fname.c_str(), isLightmap);
	textures.push_back(t);
	return t.texId;
}

GLuint TextureManager::loadTexture(unsigned char* memData, const unsigned int size, const char* modelName, int texIndex)
{
	for (auto& i : textures)
	{
		if (i.modelNameS == modelName && i.itexIndex == texIndex)
		{
			std::cout << "Loading " << modelName << ": texture already loaded, skipping." << std::endl;
			return i.texId;
		}
	}

	Texture t = Texture(memData, size, modelName, texIndex);
	textures.push_back(t);
	return t.texId;
}

unsigned int TextureManager::loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
			std::cout << "Loaded cubemap texture: " << faces[i] << std::endl;
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glGenerateTextureMipmap(textureID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);

	return textureID;
}
