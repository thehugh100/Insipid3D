#pragma once
#include <iostream>
#include <sstream>
#include <windows.h>
#include "GL\freeglut.h"
#include "HUtils.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

	struct CTextureManager
	{
		static GLuint loadTexturePNGR(const char *fname);
	};

	struct col4f
	{
		col4f()
		{
		}
		col4f(float r, float g, float b, float a)
		{
			d[0] = r;
			d[1] = g;
			d[2] = b;
			d[3] = a;
		}
		float d[4];
	};

	struct light
	{
		light(HUtils::XYZ *nPos, HUtils::XYZ nCol, float nIntensity = 100.0f)
		{
			pos = nPos;
			col = nCol;
			intensity = nIntensity;
		}
		HUtils::XYZ *pos;
		HUtils::XYZ col;
		float intensity;
	};

	struct tri
	{
		tri()
		{
			normal = HUtils::XYZ(0, 0, 0);
			t[0] = HUtils::XYZ(0, 0, 0);
			t[1] = HUtils::XYZ(0, 0, 0);
			t[2] = HUtils::XYZ(0, 0, 0);
		}
		tri(HUtils::XYZ a, HUtils::XYZ b, HUtils::XYZ c)
		{
			t[0] = a;
			t[1] = b;
			t[2] = c;
		}
		inline void calcNomral()
		{
			HUtils::XYZ d1 = HUtils::XYZ(t[1].x - t[0].x, t[1].y - t[0].y, t[1].z - t[0].z);
			HUtils::XYZ d2 = HUtils::XYZ(t[2].x - t[0].x, t[2].y - t[0].y, t[2].z - t[0].z);
			normal = HUtils::XYZ(d1.y * d2.z - d1.z * d2.y, d1.z * d2.x - d1.x * d2.z, d1.x * d2.y - d1.y * d2.x);

			float normalM = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
			normal.x /= normalM;
			normal.y /= normalM;
			normal.z /= normalM;

			col.x = (204 - rand() % 50) / 255.0f;
			col.y = (175 - rand() % 50) / 255.0f;
			col.z = (102 - rand() % 50) / 255.0f;
		}
		HUtils::XYZ normal;
		HUtils::XYZ t[3];
		HUtils::XYZ col;
		int materialId;
		float texCoords[2];
	};

	struct plane
	{
		plane(float na, float nb, float nc, float nk)
		{
			a = na;
			b = nb;
			c = nc;
			k = nk;
		}
		plane(HUtils::XYZ p1, HUtils::XYZ p2, HUtils::XYZ p3)
		{
			tris[0] = p1;
			tris[1] = p2;
			tris[2] = p3;

			HUtils::XYZ d1 = HUtils::XYZ(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
			HUtils::XYZ d2 = HUtils::XYZ(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);
			HUtils::XYZ n = HUtils::XYZ(d1.y * d2.z - d1.z * d2.y, d1.z * d2.x - d1.x * d2.z, d1.x * d2.y - d1.y * d2.x);

			k = n.x * p1.x + n.y * p1.y + n.z * p1.z;
			a = n.x;
			b = n.y;
			c = n.z;
		}
		HUtils::XYZ tris[3];
		float a, b, c, k;

		bool hit = false;
		float t;
		HUtils::XYZ hitpos = HUtils::XYZ(0, 0, 0);
	};

struct CModel
{
	CModel(std::string fname, HUtils::XYZ nOrigin, std::string mtlBase, float scale)
	{
		mtlBase_ = mtlBase;
		filename = fname;
		rotation = HUtils::XYZ(0, 0, 0);
		ang = 0;
		LoadObj(shapes, materials, error, fname.c_str(), mtlBase.c_str());
		if (error != "") { cout << error << endl; }
		origin = nOrigin;
		
		//cout << "Loaded " << fname << "\nStats:\nShapes: " << shapes.size() << "\nIndices: " << shapes[0].mesh.indices.size() << endl;
		/*//cout << "TexCoords: " << shapes[0].mesh.texcoords.size() << endl;
		if (materials.size() > 0)
		{
		cout << "Material Info: " << materials.size() << " materials\nName: " << materials[0].name << "\nAlpha Texture: " << materials[0].alpha_texname
		<< "\nAmbient Texture: " << materials[0].ambient_texname << "\nBump Map: " << materials[0].bump_texname << "\nDiffuse Texture: " << materials[0].diffuse_texname << endl;
		}*/
		
		tri temp;
		int state = 0;
		for (int i = 0; i < shapes.size(); i++)
		{
			for (int a = 0; a < shapes[i].mesh.indices.size(); a++)
			{
				int ind = shapes[i].mesh.indices[a];
				HUtils::XYZ pos(shapes[i].mesh.positions[(ind * 3)], shapes[i].mesh.positions[(ind * 3) + 1], shapes[i].mesh.positions[(ind * 3) + 2]);
		
				//if (a < shapes[i].mesh.texcoords.size())
		
				pos.x *= scale;
				pos.y *= scale;
				pos.z *= scale;
		
				//float p = 100.0f / distancef(pos.x, pos.y, pos.z, me.x * 20, me.y * 20, me.z * 20);
				temp.t[state] = pos;
				state++;
				if (state > 2)
				{
					//temp.materialId = shapes[i].mesh.material_ids[a / 3];
					//temp.texCoords[0] = shapes[i].mesh.texcoords[a / 3];
					//temp.texCoords[1] = shapes[i].mesh.texcoords[(a / 3) + 1];
					triangles.push_back(temp);
					state = 0;
				}
			}
		}
		for (int i = 0; i < triangles.size(); i++)
		{
			triangles[i].calcNomral();
			planes.push_back(plane(triangles[i].t[0], triangles[i].t[1], triangles[i].t[2]));
		}
	}
	CModel()
	{

	}
	void loadTextures()
	{
		for (int i = 0; i < materials.size(); i++)
		{
			ostringstream path;
			path << mtlBase_ << materials[i].diffuse_texname;
			textureIds.push_back(CTextureManager::loadTexturePNGR(path.str().c_str()));
		}
	}
	inline float fastDist(HUtils::XYZ a, HUtils::XYZ b)
	{
		return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z);
	}
	void color(HUtils::XYZ nCol)
	{
		for (int i = 0; i < triangles.size(); i++)
		{
			triangles[i].col = nCol;
		}
	}
	void renderWireFrame(col4f c = col4f(0.06, 0.06, 0.06, 1))
	{
		glLineWidth(3);
		glPushMatrix();
		glTranslatef(origin.x, origin.y, origin.z);
		if (!useVec)
		{
			glRotatef(rotation.x, 1, 0, 0);
			glRotatef(rotation.y, 0, 1, 0);
			glRotatef(rotation.z, 0, 0, 1);
		}
		else
		{
			glRotatef(rotationAngle, rotAxis.x, rotAxis.y, rotAxis.z);
		}
		
		glColor4f(c.d[0], c.d[1], c.d[2], c.d[3]);
		glBegin(GL_LINES);
		for (int i = 0; i < triangles.size(); i++)
		{
			glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
			glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
		
			glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
			glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
		
			glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
			glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
		}
		glEnd();
		glPopMatrix();
		glColor4f(1, 1, 1, 1);
	}
	void renderReflection()
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glPushMatrix();
		
		glTranslatef(origin.x, origin.y, origin.z);
		glScalef(1, -1, 1);
		
		glRotatef(rotation.x, 1, 0, 0);
		glRotatef(rotation.y, 0, 1, 0);
		glRotatef(rotation.z, 0, 0, 1);
		
		glBegin(GL_TRIANGLES);
		
		for (int i = 0; i < triangles.size(); i++)
		{
			glColor4f(triangles[i].col.x, triangles[i].col.x, triangles[i].col.x, 0.4);
			glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
			glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
			glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
		}
		glEnd();
		//glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glColor4f(1, 1, 1, 1);
	}
	void renderRaw()
	{
		glPushMatrix();
		glTranslatef(origin.x, origin.y, origin.z);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < triangles.size(); i++)
		{
			glColor4f(triangles[i].col.x, triangles[i].col.x, triangles[i].col.x, 1);
			glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
			glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
			glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
		}
		glEnd();
		glPopMatrix();
	}
	void renderViewModel(int wireFrame)
	{
		glEnable(GL_CULL_FACE);
		glPushMatrix();
		glTranslatef(origin.x, origin.y, origin.z);
		if (!useVec)
		{
			glRotatef(rotation.x, 1, 0, 0);
			glRotatef(rotation.y, 0, 1, 0);
			glRotatef(rotation.z, 0, 0, 1);
		}
		else
		{
			glRotatef(rotationAngle, rotAxis.x, rotAxis.y, rotAxis.z);
		}
		glBegin(GL_TRIANGLES);
		int doLight = lights.size() > 0;
		if (!doLight)
		{
			if (wireFrame == 2 || wireFrame == 3)
			{
				for (int i = 0; i < triangles.size(); i++)
				{
					glNormal3f(triangles[i].normal.x, triangles[i].normal.y, triangles[i].normal.z);
		
					if (fastDist(triangles[i].normal, HUtils::XYZ(0, 1, 0)) < 0.2)
					{
						glColor4f(0.2, 0.2, 0.2, 1);
						glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
						glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
						glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
					}
					else
					{
						float p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[0])) / 10;
						if (p > 0.9)
							p = 0.9;
						glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
						glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
						p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[1])) / 10;
						if (p > 0.9)
							p = 0.9;
						glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
						glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
						p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[2])) / 10;
						if (p > 0.9)
							p = 0.9;
						glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
						glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
					}
				}
			}
			else
			{
				float dt = (sin((dayTime/*Day time*/ + 180.0f) * 3.1415 / 180.0f) / 2.2f) + 0.6;
		
				for (int i = 0; i < triangles.size(); i++)
				{
					glNormal3f(triangles[i].normal.x, triangles[i].normal.y, triangles[i].normal.z);
					//float p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[0])) / 10;
					float p = 1 * dt;
					HUtils::XYZ d = HUtils::XYZ(sin(dayTime/*Day time*/ * 3.1415 / 180.0f) * 1.0f, 1, cos(dayTime/*Day time*/ * 3.1415 / 180.0f) * 1.0f);
					d.normalize();
		
					HUtils::XYZ nNorm = yRotate(triangles[i].normal, 0);
					//HUtils::XYZ nNorm = triangles[i].normal;
		
					p *= (0.1 + Dot(d, nNorm));
					if (p > 0.8)
						p = 0.8;
		
					if (p < 0.2)
						p = 0.2;
					glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
		
					glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
		
					//	p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[1])) / 10;
					//	if (p > 0.9)
					//		p = 0.9;
					//	glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
		
					glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
		
					//	p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[2])) / 10;
					//	if (p > 0.9)
					//		p = 0.9;
		
					//glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
					glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
				}
			}
		}
		else
		{
			for (int i = 0; i < triangles.size(); i++)
			{
				glNormal3f(triangles[i].normal.x, triangles[i].normal.y, triangles[i].normal.z);
		
				HUtils::XYZ thisCol[3];
		
				for (int a = 0; a < lights.size(); a++)
				{
					for (int c = 0; c < 3; c++)
					{
						thisCol[c] = triangles[i].col;
						float d = pow(lights[a].intensity / fastDist(triangles[i].t[c] + origin, *lights[a].pos), 2);
						thisCol[c] *= lights[a].col * d;
					}
				}
		
				glColor4f(thisCol[0].x, thisCol[0].y, thisCol[0].z, 1);
				glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
				glColor4f(thisCol[1].x, thisCol[1].y, thisCol[1].z, 1);
				glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
				glColor4f(thisCol[2].x, thisCol[2].y, thisCol[2].z, 1);
				glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
			}
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		glDisable(GL_CULL_FACE);
		
		if (wireFrame == 1 || wireFrame == 3)
			renderWireFrame();
	}
	void render(int wireFrame)
	{
		glEnable(GL_CULL_FACE);
		glPushMatrix();
		glTranslatef(origin.x, origin.y, origin.z);
		if (!useVec)
		{
			glRotatef(rotation.x, 1, 0, 0);
			glRotatef(rotation.y, 0, 1, 0);
			glRotatef(rotation.z, 0, 0, 1);
		}
		else
		{
			glRotatef(rotationAngle, rotAxis.x, rotAxis.y, rotAxis.z);
		}
		glBegin(GL_TRIANGLES);
		int doLight = lights.size() > 0;
		if (!doLight)
		{
			if (wireFrame == 2 || wireFrame == 3)
			{
				for (int i = 0; i < triangles.size(); i++)
				{
					glNormal3f(triangles[i].normal.x, triangles[i].normal.y, triangles[i].normal.z);
		
					if (fastDist(triangles[i].normal, HUtils::XYZ(0, 1, 0)) < 0.2)
					{
						glColor4f(0.2, 0.2, 0.2, 1);
						glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
						glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
						glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
					}
					else
					{
						float p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[0])) / 10;
						if (p > 0.9)
							p = 0.9;
						glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
						glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
						p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[1])) / 10;
						if (p > 0.9)
							p = 0.9;
						glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
						glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
						p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[2])) / 10;
						if (p > 0.9)
							p = 0.9;
						glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
						glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
					}
				}
			}
			else
			{
				float dt = (sin((dayTime/*Day time*/ + 180.0f) * 3.1415 / 180.0f) / 2.2f) + 0.6;
		
				for (int i = 0; i < triangles.size(); i++)
				{
					glNormal3f(triangles[i].normal.x, triangles[i].normal.y, triangles[i].normal.z);
					//float p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[0])) / 10;
					float p = 1 * dt;
					HUtils::XYZ d = HUtils::XYZ(sin(dayTime/*Day time*/ * 3.1415 / 180.0f) * 1.0f, 1, cos(dayTime/*Day time*/ * 3.1415 / 180.0f) * 1.0f);
					d.normalize();
		
					//HUtils::XYZ nNorm = rotVec(triangles[i].normal, me.yaw);
					HUtils::XYZ nNorm = triangles[i].normal;
		
					p *= (0.4 + Dot(d, nNorm));
					if (p > 0.8)
						p = 0.8;
		
					if (p < 0.2)
						p = 0.2;
					glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
		
					glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
		
					//	p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[1])) / 10;
					//	if (p > 0.9)
					//		p = 0.9;
					//	glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
		
					glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
		
					//	p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[2])) / 10;
					//	if (p > 0.9)
					//		p = 0.9;
		
					//glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
					glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
				}
			}
		}
		else
		{
			for (int i = 0; i < triangles.size(); i++)
			{
				glNormal3f(triangles[i].normal.x, triangles[i].normal.y, triangles[i].normal.z);
		
				HUtils::XYZ thisCol[3];
		
				for (int a = 0; a < lights.size(); a++)
				{
					for (int c = 0; c < 3; c++)
					{
						thisCol[c] = triangles[i].col;
						float d = pow(lights[a].intensity / fastDist(triangles[i].t[c] + origin, *lights[a].pos), 2);
						thisCol[c] *= lights[a].col * d;
					}
				}
		
				glColor4f(thisCol[0].x, thisCol[0].y, thisCol[0].z, 1);
				glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
				glColor4f(thisCol[1].x, thisCol[1].y, thisCol[1].z, 1);
				glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
				glColor4f(thisCol[2].x, thisCol[2].y, thisCol[2].z, 1);
				glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
			}
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		glDisable(GL_CULL_FACE);
		
		if (wireFrame == 1 || wireFrame == 3)
			renderWireFrame();
	}

	void mapColorize()
	{
		for (int i = 0; i < triangles.size(); i++)
		{
			if (abs(triangles[i].normal.y) < 0.2)
				triangles[i].col = HUtils::XYZ(0.7, 0.7, 0.7) + HUtils::randVec() * 0.1;
		
			if (abs(triangles[i].normal.x) < 0.2 && abs(triangles[i].normal.z) < 0.2 && abs(triangles[i].normal.y) > 0)
				triangles[i].col = HUtils::XYZ(0.2, 0.2, 0.2);
		}
	}

	std::string mtlBase_;
	std::string filename;
	std::string error;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	vector<tri> triangles;
	vector<plane> planes;
	HUtils::XYZ origin;

	HUtils::XYZ rotAxis;
	float rotationAngle;
	int useVec;

	float dayTime = 270;

	HUtils::XYZ rotation;
	float ang;
	HUtils::XYZ me;
	vector<GLuint> textureIds;
	vector<light> lights;
};

inline float Qsqrt(float number)
{
	/*long i;
	float x2, y;
	x2 = number * 0.5F;
	y = number;
	i = *(long *)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float *)&i;
	//for (int k = 0; i < 1; k++)
	y = y * (1.5F - (x2 * y * y));
	return 1.0f / y;*/


	//SSESqrt_Recip_Times_X(&res, &number);
	//sqrtf()
	return sqrtf(number);
}

inline float distancef(float x1, float y1, float z1, float x2, float y2, float z2)
{
	return Qsqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - z1, 2));
}

HUtils::XYZ projP = HUtils::XYZ(0, 0, 0);
// 
//inline float fast_s(float f)
//{
//	int i = 0x1FBD1DF5 + (*(int*)&f >> 1);
//	return *(float*)&i;
//}
//inline float triArea32(HUtils::XYZ *p0, HUtils::XYZ *p1, HUtils::XYZ *p2)
//{
//	float d1 = (p1->y - p0->y)*(p2->z - p0->z) - (p1->z - p0->z)*(p2->y - p0->y);
//	float d2 = (p1->z - p0->z)*(p2->x - p0->x) - (p1->x - p0->x)*(p2->z - p0->z);
//	float d3 = (p1->x - p0->x)*(p2->y - p0->y) - (p1->y - p0->y)*(p2->x - p0->x);
//
//	return fast_s(d1*d1 + d2*d2 + d3*d3);
//}
//inline bool isInside2(HUtils::XYZ *p, HUtils::XYZ *t1, HUtils::XYZ *t2, HUtils::XYZ *t3)
//{
//	float actArea = triArea3(t1, t2, t3) * 1.0545f;
//	float a1 = triArea3(p, t1, t2);
//	if (a1 > actArea) return false;
//	float a2 = triArea3(p, t2, t3);
//	if (a2 > actArea) return false;
//	float a3 = triArea3(p, t3, t1);
//	if (a3 > actArea) return false;
//	return a1 + a2 + a3 <= actArea;
//}

inline float triArea3(HUtils::XYZ *p0, HUtils::XYZ *p1, HUtils::XYZ *p2)
{
	float d1 = (p1->y - p0->y)*(p2->z - p0->z) - (p1->z - p0->z)*(p2->y - p0->y);
	float d2 = (p1->z - p0->z)*(p2->x - p0->x) - (p1->x - p0->x)*(p2->z - p0->z);
	float d3 = (p1->x - p0->x)*(p2->y - p0->y) - (p1->y - p0->y)*(p2->x - p0->x);

	return sqrtf(d1*d1 + d2*d2 + d3*d3);
}
inline bool isInside(HUtils::XYZ *p, HUtils::XYZ *t1, HUtils::XYZ *t2, HUtils::XYZ *t3)
{
	float actArea = triArea3(t1, t2, t3) * 1.005f;
	float a1 = triArea3(p, t1, t2);
	if (a1 > actArea) return false;
	float a2 = triArea3(p, t2, t3);
	if (a2 > actArea) return false;
	float a3 = triArea3(p, t3, t1);
	if (a3 > actArea) return false;
	return a1 + a2 + a3 <= actArea;
}


struct traceProperties
{
	float t;
	HUtils::XYZ hitPos;
	bool hit;
	tri _tri;
};

struct trace
{
	bool didHit;
	HUtils::XYZ hitPos;
	HUtils::XYZ hitNormal;
	float distance;
	tri _tri;
	bool traceFailed;
};

trace _2k17trace(HUtils::XYZ eye, HUtils::XYZ eyedir, CModel *mesh)
{
	trace q = { 0 };
	eyedir.normalize();
	for (int i = 0; i < 1024; ++i)
	{
		eye += eyedir / 4;

		for (int a = 0; a < mesh->triangles.size(); ++a)
		{

		}
	}
	return q;
}

inline trace rayTrace(HUtils::XYZ eye, HUtils::XYZ eyeDir, CModel *mesh)
{
	eyeDir.normalize();
	trace newTrace;
	newTrace.traceFailed = 0;
	newTrace.didHit = 0;
	newTrace.hitNormal = HUtils::XYZ();
	newTrace.hitPos = HUtils::XYZ();
	vector<traceProperties> hitplanes;
	hitplanes.reserve(mesh->planes.size());
	plane *p;
	for (int i = 0; i < mesh->planes.size(); i++)
	{
		mesh->planes[i].hit = false;
		mesh->planes[i].hitpos = HUtils::XYZ(0, 0, 0);

		traceProperties tp;
		tp.hitPos = HUtils::XYZ();
		tp.t = 0.0f;
		p = &mesh->planes[i];
		float t = (p->k - p->a * eye.x - p->b * eye.y - p->c * eye.z) / (p->a * eyeDir.x + p->b * eyeDir.y + p->c * eyeDir.z);
		if (t < 0.0f)
			continue;
		HUtils::XYZ intPoint = HUtils::XYZ(eye.x + eyeDir.x * t, eye.y + eyeDir.y * t, eye.z + eyeDir.z * t);
		tp.hit = isInside(&intPoint, &p->tris[0], &p->tris[1], &p->tris[2]);
		tp.t = t;
		tp.hitPos = intPoint;
		tp._tri = tri(mesh->triangles[i].t[0] + mesh->origin, mesh->triangles[i].t[1] + mesh->origin, mesh->triangles[i].t[2] + mesh->origin);
		if (tp.hit)
			hitplanes.push_back(tp);
	}

	float min = INT_MAX;
	traceProperties minp;
	bool found = false;
	for (int i = 0; i < hitplanes.size(); ++i)
	{
		if (hitplanes[i].t > 0.0f && hitplanes[i].t < min)
		{
			min = hitplanes[i].t;
			minp = hitplanes[i];
			found = true;
		}
	}
	if (found)
	{
		//minp->hit = true;
		projP = minp.hitPos;
		newTrace.hitPos = minp.hitPos;
		newTrace.didHit = 1;
		minp._tri.calcNomral();
		newTrace.hitNormal = minp._tri.normal;
		newTrace._tri = minp._tri;
		newTrace.distance = distancef(eye.x, eye.y, eye.z, newTrace.hitPos.x, newTrace.hitPos.y, newTrace.hitPos.z);
		return newTrace;
	}
	return newTrace;
}