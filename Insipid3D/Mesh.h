#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assimp\scene.h>
#include <assimp\mesh.h>
#include <assimp\Importer.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <string>
#include "TextureManager.h"

class Mesh
{
public:
	struct MeshEntry {
		static enum BUFFERS {
			VERTEX_BUFFER, TEXCOORD_BUFFER, NORMAL_BUFFER, INDEX_BUFFER, LIGHTMAP_BUFFER
		};
		GLuint vao;
		GLuint vbo[5];
		
		GLuint texID;
		int materialID;

		aiMesh* meshRef;

		glm::vec3 minBound;
		glm::vec3 maxBound;

		unsigned int elementCount;

		MeshEntry(aiMesh* mesh);
		~MeshEntry();

		void load(aiMesh* mesh);
		void render(int renderMode = GL_TRIANGLES, int lightmap = -1);
	};
	const aiScene* scene;
	Assimp::Importer importer;
	std::vector<MeshEntry*> meshEntries;
	bool hasLightmap;
	GLuint lightmap;
	std::string filename;
public:
	Mesh(const char* filename, TextureManager *texMan, unsigned int flags = 0);
	~Mesh(void);

	void render(int renderMode = GL_TRIANGLES);
};