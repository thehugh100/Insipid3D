#pragma once

#include "GL/glew.h"
#include <GLFW/glfw3.h>
GLuint LoadShader(const char* vertex_path, const char* fragment_path, const char* geometry_path = "\0");