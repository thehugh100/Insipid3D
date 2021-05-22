#include "Mesh.h"
#include <assimp\postprocess.h>
#include <glm/glm.hpp>

Mesh::MeshEntry::MeshEntry(aiMesh* mesh)
{
	meshRef = mesh;

	vbo[VERTEX_BUFFER] = NULL;
	vbo[TEXCOORD_BUFFER] = NULL;
	vbo[NORMAL_BUFFER] = NULL;
	vbo[INDEX_BUFFER] = NULL;
	vbo[LIGHTMAP_BUFFER] = NULL;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	elementCount = mesh->mNumFaces * 4;

	if (mesh->HasPositions())
	{
		minBound = glm::vec3(mesh->mVertices[0].x, mesh->mVertices[0].y, mesh->mVertices[0].z);
		maxBound = glm::vec3(mesh->mVertices[0].x, mesh->mVertices[0].y, mesh->mVertices[0].z);

		float* vertices = new float[mesh->mNumVertices * 3];
		for (int i = 0; i < mesh->mNumVertices; ++i) {
			vertices[i * 3] = mesh->mVertices[i].x;
			vertices[i * 3 + 1] = mesh->mVertices[i].y;
			vertices[i * 3 + 2] = mesh->mVertices[i].z;

			minBound.x = glm::min(minBound.x, mesh->mVertices[i].x);
			minBound.y = glm::min(minBound.y, mesh->mVertices[i].y);
			minBound.z = glm::min(minBound.z, mesh->mVertices[i].z);

			maxBound.x = glm::max(maxBound.x, mesh->mVertices[i].x);
			maxBound.y = glm::max(maxBound.y, mesh->mVertices[i].y);
			maxBound.z = glm::max(maxBound.z, mesh->mVertices[i].z);
		}

		glGenBuffers(1, &vbo[VERTEX_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[VERTEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		delete[] vertices;
	}

	if (mesh->HasTextureCoords(0))
	{
		int uvChannels = mesh->GetNumUVChannels();

		float* texCoords = new float[mesh->mNumVertices * 2];
		for (int i = 0; i < mesh->mNumVertices; ++i) {
			texCoords[i * 2] = mesh->mTextureCoords[uvChannels-1][i].x;
			texCoords[i * 2 + 1] = mesh->mTextureCoords[uvChannels-1][i].y;
		}

		glGenBuffers(1, &vbo[TEXCOORD_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 2 * mesh->mNumVertices * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

		glVertexAttribPointer(TEXCOORD_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);

		delete[] texCoords;
	}

	if (mesh->HasNormals()) {
		float* normals = new float[mesh->mNumVertices * 3];
		for (int i = 0; i < mesh->mNumVertices; ++i) {
			normals[i * 3] = mesh->mNormals[i].x;
			normals[i * 3 + 1] = mesh->mNormals[i].y;
			normals[i * 3 + 2] = mesh->mNormals[i].z;
		}

		glGenBuffers(1, &vbo[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), normals, GL_STATIC_DRAW);

		glVertexAttribPointer(NORMAL_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);

		delete[] normals;
	}

	if (mesh->HasFaces())
	{
		unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
		for (int i = 0; i < mesh->mNumFaces; ++i) {
			indices[i * 3] = mesh->mFaces[i].mIndices[0];
			indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
			indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
		}

		glGenBuffers(1, &vbo[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mesh->mNumFaces * sizeof(GLuint), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(INDEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(3);

		delete[] indices;
	}

	if (mesh->HasTextureCoords(0))
	{
		int uvChannels = mesh->GetNumUVChannels();
		float* texCoords = new float[mesh->mNumVertices * 2];
		for (int i = 0; i < mesh->mNumVertices; ++i) {
			texCoords[i * 2] = mesh->mTextureCoords[0][i].x;
			texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		}

		glGenBuffers(1, &vbo[LIGHTMAP_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[LIGHTMAP_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 2 * mesh->mNumVertices * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

		glVertexAttribPointer(LIGHTMAP_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(4);

		delete[] texCoords;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

/**
*	Deletes the allocated OpenGL buffers
**/
Mesh::MeshEntry::~MeshEntry() {
	if (vbo[VERTEX_BUFFER]) {
		glDeleteBuffers(1, &vbo[VERTEX_BUFFER]);
	}

	if (vbo[TEXCOORD_BUFFER]) {
		glDeleteBuffers(1, &vbo[TEXCOORD_BUFFER]);
	}

	if (vbo[NORMAL_BUFFER]) {
		glDeleteBuffers(1, &vbo[NORMAL_BUFFER]);
	}

	if (vbo[INDEX_BUFFER]) {
		glDeleteBuffers(1, &vbo[INDEX_BUFFER]);
	}

	if (vbo[LIGHTMAP_BUFFER]) {
		glDeleteBuffers(1, &vbo[LIGHTMAP_BUFFER]);
	}

	glDeleteVertexArrays(1, &vao);
}

/**
*	Renders this MeshEntry
**/
void Mesh::MeshEntry::render(int renderMode, int lightmap)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	if (lightmap != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, lightmap);
	}

	glBindVertexArray(vao);

	glDrawElements(renderMode, elementCount, GL_UNSIGNED_INT, (GLvoid*)0);
	glBindVertexArray(0);
}

void Mesh::MeshEntry::renderExternTex(int texIDExtern, int renderMode)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texIDExtern);

	glBindVertexArray(vao);

	glDrawElements(renderMode, elementCount, GL_UNSIGNED_INT, (GLvoid*)0);
	glBindVertexArray(0);
}

/**
*	Mesh constructor, loads the specified filename if supported by Assimp
**/
Mesh::Mesh(const char* filename, TextureManager* texMan, unsigned int flags)
	:filename(filename)
{
	std::cout << "Loading mesh: " << filename << std::endl;
	hasLightmap = 0;
	lightmap = -1;
	scene = importer.ReadFile(filename, aiProcess_PreTransformVertices | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | flags);
	if (!scene) {
		printf("Unable to load mesh: %s\n", importer.GetErrorString());
		return;
	}

	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		meshEntries.emplace_back(new Mesh::MeshEntry(scene->mMeshes[i]));
	}

	for (int i = 0; i < scene->mNumMaterials; ++i)
	{
		int textures = scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE);
		for (int j = 0; j < textures; ++j)
		{
			aiString path;
			scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, j, &path);
			std::string t = path.C_Str();
			if (t[0] == '*' && t.length() > 1)
			{
				int textureIndex = atoi(t.substr(1).c_str()); //internal texture index
				aiTexture *t = scene->mTextures[textureIndex];

				unsigned int size = t->mWidth;
				if (t->mHeight != 0) size = t->mWidth * t->mHeight;

				GLuint texIdVal = texMan->loadTexture((unsigned char*)t->pcData, size, filename, textureIndex);

				for (auto& m : meshEntries)
					if (m->meshRef->mMaterialIndex == i)
						m->texID = texIdVal;
			}
			else
			{
				std::cout << "External Texture Required for material " << i << std::endl;
				GLuint texIdVal = texMan->loadTexture((std::string("textures/") + path.C_Str()).c_str());
				for (auto& m : meshEntries)
					if (m->meshRef->mMaterialIndex == i)
						m->texID = texIdVal;
			}
		}
	}
}

/**
*	Clears all loaded MeshEntries
**/
Mesh::~Mesh(void)
{
	for (int i = 0; i < meshEntries.size(); ++i) {
		delete meshEntries.at(i);
	}
	meshEntries.clear();
}

/**
*	Renders all loaded MeshEntries
**/
void Mesh::render(int renderMode)
{
	for (int i = 0; i < meshEntries.size(); ++i)
	{
		int li = -1;
		if (hasLightmap) li = lightmap;

		meshEntries.at(i)->render(renderMode, li);
	}
}

void Mesh::renderExternTex(int texIDExtern, int renderMode)
{
	for (int i = 0; i < meshEntries.size(); ++i)
	{
		meshEntries.at(i)->renderExternTex(texIDExtern, renderMode);
	}
}
