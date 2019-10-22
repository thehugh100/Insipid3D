#pragma once
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>

#include "GL/glew.h"
#include <GLFW/glfw3.h>

struct Character
{
	GLuint     TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	GLuint     Advance;
};

struct Font
{
	Font(FT_Library *ft, std::string fontName, int size);

	FT_Face face;
	std::map<GLchar, Character> Characters;
	std::string fontName;
	int size;
	void renderText(GLuint shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, glm::mat4 projection);
	GLuint VAO, VBO;
};

struct FontManager
{
	FontManager();
	~FontManager();

	Font* getFont(std::string fontName, int size);
	Font* addFont(std::string fontName, int size);

	std::vector<Font*> fonts;

	Font* defaultFont;
	FT_Library ft;
};