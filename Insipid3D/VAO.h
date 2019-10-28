#pragma once
#include "GL/glew.h"
#include <GLFW/glfw3.h>

struct vertexArrayObject
{
	GLuint vao;
	GLuint vbo;

	vertexArrayObject()
	{
	}

	vertexArrayObject(float* v, int size)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, size, v, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};