//#include "model.h"
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//#include <iostream>
//
//
//GLuint CTextureManager::loadTexturePNGR(const char *fname)
//{
//	cout << "fname: " << fname << endl;
//	int x, y, n;
//	unsigned char *data = stbi_load(fname, &x, &y, &n, 0);
//	GLuint textureId;
//	glGenTextures(1, &textureId);
//	glBindTexture(GL_TEXTURE_2D, textureId);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, x, y, GL_RGBA, GL_UNSIGNED_BYTE, data);
//	return textureId;
//}
//
//void CModel::mapColorize()
//{
//	for (int i = 0; i < triangles.size(); i++)
//	{
//		if (abs(triangles[i].normal.y) < 0.2)
//			triangles[i].col = HUtils::XYZ(0.7, 0.7, 0.7) + HUtils::randVec() * 0.1;
//
//		if (abs(triangles[i].normal.x) < 0.2 && abs(triangles[i].normal.z) < 0.2 && abs(triangles[i].normal.y) > 0)
//			triangles[i].col = HUtils::XYZ(0.2, 0.2, 0.2);
//	}
//}
//
//CModel::CModel(std::string fname, HUtils::XYZ nOrigin, std::string mtlBase, float scale)
//{
//	mtlBase_ = mtlBase;
//	filename = fname;
//	rotation = HUtils::XYZ(0, 0, 0);
//	ang = 0;
//	LoadObj(shapes, materials, error, fname.c_str(), mtlBase.c_str());
//	cout << error << endl;
//	origin = nOrigin;
//
//	//cout << "Loaded " << fname << "\nStats:\nShapes: " << shapes.size() << "\nIndices: " << shapes[0].mesh.indices.size() << endl;
//	/*//cout << "TexCoords: " << shapes[0].mesh.texcoords.size() << endl;
//	if (materials.size() > 0)
//	{
//	cout << "Material Info: " << materials.size() << " materials\nName: " << materials[0].name << "\nAlpha Texture: " << materials[0].alpha_texname
//	<< "\nAmbient Texture: " << materials[0].ambient_texname << "\nBump Map: " << materials[0].bump_texname << "\nDiffuse Texture: " << materials[0].diffuse_texname << endl;
//	}*/
//
//	tri temp;
//	int state = 0;
//	for (int i = 0; i < shapes.size(); i++)
//	{
//		for (int a = 0; a < shapes[i].mesh.indices.size(); a++)
//		{
//			int ind = shapes[i].mesh.indices[a];
//			HUtils::XYZ pos(shapes[i].mesh.positions[(ind * 3)], shapes[i].mesh.positions[(ind * 3) + 1], shapes[i].mesh.positions[(ind * 3) + 2]);
//
//			//if (a < shapes[i].mesh.texcoords.size())
//
//			pos.x *= scale;
//			pos.y *= scale;
//			pos.z *= scale;
//
//			//float p = 100.0f / distancef(pos.x, pos.y, pos.z, me.x * 20, me.y * 20, me.z * 20);
//			temp.t[state] = pos;
//			state++;
//			if (state > 2)
//			{
//				//temp.materialId = shapes[i].mesh.material_ids[a / 3];
//				//temp.texCoords[0] = shapes[i].mesh.texcoords[a / 3];
//				//temp.texCoords[1] = shapes[i].mesh.texcoords[(a / 3) + 1];
//				triangles.push_back(temp);
//				state = 0;
//			}
//		}
//	}
//	for (int i = 0; i < triangles.size(); i++)
//	{
//		triangles[i].calcNomral();
//		planes.push_back(plane(triangles[i].t[0], triangles[i].t[1], triangles[i].t[2]));
//	}
//}
//CModel::CModel()
//{
//
//}
//
//void CModel::loadTextures()
//{
//	for (int i = 0; i < materials.size(); i++)
//	{
//		ostringstream path;
//		path << mtlBase_ << materials[i].diffuse_texname;
//		textureIds.push_back(CTextureManager::loadTexturePNGR(path.str().c_str()));
//	}
//}
//inline float CModel::fastDist(HUtils::XYZ a, HUtils::XYZ b)
//{
//	return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z);
//}
//
//void CModel::color(HUtils::XYZ nCol)
//{
//	for (int i = 0; i < triangles.size(); i++)
//	{
//		triangles[i].col = nCol;
//	}
//}
//
//void CModel::renderWireFrame(col4f c)
//{
//	glLineWidth(3);
//	glPushMatrix();
//	glTranslatef(origin.x, origin.y, origin.z);
//	if (!useVec)
//	{
//		glRotatef(rotation.x, 1, 0, 0);
//		glRotatef(rotation.y, 0, 1, 0);
//		glRotatef(rotation.z, 0, 0, 1);
//	}
//	else
//	{
//		glRotatef(rotationAngle, rotAxis.x, rotAxis.y, rotAxis.z);
//	}
//
//	glColor4f(c.d[0], c.d[1], c.d[2], c.d[3]);
//	glBegin(GL_LINES);
//	for (int i = 0; i < triangles.size(); i++)
//	{
//		glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
//		glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
//
//		glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
//		glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
//
//		glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
//		glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
//	}
//	glEnd();
//	glPopMatrix();
//	glColor4f(1, 1, 1, 1);
//}
//
//void CModel::renderReflection()
//{
//	glDisable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
//	glPushMatrix();
//
//	glTranslatef(origin.x, origin.y, origin.z);
//	glScalef(1, -1, 1);
//
//	glRotatef(rotation.x, 1, 0, 0);
//	glRotatef(rotation.y, 0, 1, 0);
//	glRotatef(rotation.z, 0, 0, 1);
//
//	glBegin(GL_TRIANGLES);
//
//	for (int i = 0; i < triangles.size(); i++)
//	{
//		glColor4f(triangles[i].col.x, triangles[i].col.x, triangles[i].col.x, 0.4);
//		glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
//		glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
//		glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
//	}
//	glEnd();
//	//glDisable(GL_TEXTURE_2D);
//	glPopMatrix();
//	glDisable(GL_CULL_FACE);
//	glEnable(GL_DEPTH_TEST);
//	glColor4f(1, 1, 1, 1);
//}
//
//
//void CModel::renderRaw()
//{
//	glPushMatrix();
//	glTranslatef(origin.x, origin.y, origin.z);
//	glBegin(GL_TRIANGLES);
//	for (int i = 0; i < triangles.size(); i++)
//	{
//		glColor4f(triangles[i].col.x, triangles[i].col.x, triangles[i].col.x, 1);
//		glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
//		glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
//		glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
//	}
//	glEnd();
//	glPopMatrix();
//}
//void CModel::renderViewModel(int wireFrame)
//{
//	glEnable(GL_CULL_FACE);
//	glPushMatrix();
//	glTranslatef(origin.x, origin.y, origin.z);
//	if (!useVec)
//	{
//		glRotatef(rotation.x, 1, 0, 0);
//		glRotatef(rotation.y, 0, 1, 0);
//		glRotatef(rotation.z, 0, 0, 1);
//	}
//	else
//	{
//		glRotatef(rotationAngle, rotAxis.x, rotAxis.y, rotAxis.z);
//	}
//	glBegin(GL_TRIANGLES);
//	int doLight = lights.size() > 0;
//	if (!doLight)
//	{
//		if (wireFrame == 2 || wireFrame == 3)
//		{
//			for (int i = 0; i < triangles.size(); i++)
//			{
//				glNormal3f(triangles[i].normal.x, triangles[i].normal.y, triangles[i].normal.z);
//
//				if (fastDist(triangles[i].normal, HUtils::XYZ(0, 1, 0)) < 0.2)
//				{
//					glColor4f(0.2, 0.2, 0.2, 1);
//					glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
//					glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
//					glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
//				}
//				else
//				{
//					float p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[0])) / 10;
//					if (p > 0.9)
//						p = 0.9;
//					glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
//					glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
//					p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[1])) / 10;
//					if (p > 0.9)
//						p = 0.9;
//					glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
//					glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
//					p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[2])) / 10;
//					if (p > 0.9)
//						p = 0.9;
//					glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
//					glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
//				}
//			}
//		}
//		else
//		{
//			float dt = (sin((180/*Day time*/ + 180.0f) * 3.1415 / 180.0f) / 2.2f) + 0.6;
//
//			for (int i = 0; i < triangles.size(); i++)
//			{
//				glNormal3f(triangles[i].normal.x, triangles[i].normal.y, triangles[i].normal.z);
//				//float p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[0])) / 10;
//				float p = 1 * dt;
//				HUtils::XYZ d = HUtils::XYZ(sin(180/*Day time*/ * 3.1415 / 180.0f) * 1.0f, 1, cos(180/*Day time*/ * 3.1415 / 180.0f) * 1.0f);
//				d.normalize();
//
//				HUtils::XYZ nNorm = yRotate(triangles[i].normal, 0);
//				//HUtils::XYZ nNorm = triangles[i].normal;
//
//				p *= (0.1 + Dot(d, nNorm));
//				if (p > 0.8)
//					p = 0.8;
//
//				if (p < 0.2)
//					p = 0.2;
//				glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
//
//				glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
//
//				//	p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[1])) / 10;
//				//	if (p > 0.9)
//				//		p = 0.9;
//				//	glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
//
//				glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
//
//				//	p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[2])) / 10;
//				//	if (p > 0.9)
//				//		p = 0.9;
//
//				//glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
//				glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
//			}
//		}
//	}
//	else
//	{
//		for (int i = 0; i < triangles.size(); i++)
//		{
//			glNormal3f(triangles[i].normal.x, triangles[i].normal.y, triangles[i].normal.z);
//
//			HUtils::XYZ thisCol[3];
//
//			for (int a = 0; a < lights.size(); a++)
//			{
//				for (int c = 0; c < 3; c++)
//				{
//					thisCol[c] = triangles[i].col;
//					float d = pow(lights[a].intensity / fastDist(triangles[i].t[c] + origin, *lights[a].pos), 2);
//					thisCol[c] *= lights[a].col * d;
//				}
//			}
//
//			glColor4f(thisCol[0].x, thisCol[0].y, thisCol[0].z, 1);
//			glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
//			glColor4f(thisCol[1].x, thisCol[1].y, thisCol[1].z, 1);
//			glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
//			glColor4f(thisCol[2].x, thisCol[2].y, thisCol[2].z, 1);
//			glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
//		}
//	}
//	glEnd();
//	glDisable(GL_TEXTURE_2D);
//	glPopMatrix();
//	glDisable(GL_CULL_FACE);
//
//	if (wireFrame == 1 || wireFrame == 3)
//		renderWireFrame();
//}
//void CModel::render(int wireFrame)
//{
//	glEnable(GL_CULL_FACE);
//	glPushMatrix();
//	glTranslatef(origin.x, origin.y, origin.z);
//	if (!useVec)
//	{
//		glRotatef(rotation.x, 1, 0, 0);
//		glRotatef(rotation.y, 0, 1, 0);
//		glRotatef(rotation.z, 0, 0, 1);
//	}
//	else
//	{
//		glRotatef(rotationAngle, rotAxis.x, rotAxis.y, rotAxis.z);
//	}
//	glBegin(GL_TRIANGLES);
//	int doLight = lights.size() > 0;
//	if (!doLight)
//	{
//		if (wireFrame == 2 || wireFrame == 3)
//		{
//			for (int i = 0; i < triangles.size(); i++)
//			{
//				glNormal3f(triangles[i].normal.x, triangles[i].normal.y, triangles[i].normal.z);
//
//				if (fastDist(triangles[i].normal, HUtils::XYZ(0, 1, 0)) < 0.2)
//				{
//					glColor4f(0.2, 0.2, 0.2, 1);
//					glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
//					glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
//					glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
//				}
//				else
//				{
//					float p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[0])) / 10;
//					if (p > 0.9)
//						p = 0.9;
//					glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
//					glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
//					p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[1])) / 10;
//					if (p > 0.9)
//						p = 0.9;
//					glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
//					glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
//					p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[2])) / 10;
//					if (p > 0.9)
//						p = 0.9;
//					glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
//					glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
//				}
//			}
//		}
//		else
//		{
//			float dt = (sin((180/*Day time*/ + 180.0f) * 3.1415 / 180.0f) / 2.2f) + 0.6;
//
//			for (int i = 0; i < triangles.size(); i++)
//			{
//				glNormal3f(triangles[i].normal.x, triangles[i].normal.y, triangles[i].normal.z);
//				//float p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[0])) / 10;
//				float p = 1 * dt;
//				HUtils::XYZ d = HUtils::XYZ(sin(180/*Day time*/ * 3.1415 / 180.0f) * 1.0f, 1, cos(180/*Day time*/ * 3.1415 / 180.0f) * 1.0f);
//				d.normalize();
//
//				//HUtils::XYZ nNorm = rotVec(triangles[i].normal, me.yaw);
//				HUtils::XYZ nNorm = triangles[i].normal;
//
//				p *= (0.4 + Dot(d, nNorm));
//				if (p > 0.8)
//					p = 0.8;
//
//				if (p < 0.2)
//					p = 0.2;
//				glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
//
//				glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
//
//				//	p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[1])) / 10;
//				//	if (p > 0.9)
//				//		p = 0.9;
//				//	glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
//
//				glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
//
//				//	p = 0.5 + (100 / fastDist(HUtils::XYZ(me.x, me.y, me.z), triangles[i].t[2])) / 10;
//				//	if (p > 0.9)
//				//		p = 0.9;
//
//				//glColor4f(p * triangles[i].col.x, p * triangles[i].col.y, p * triangles[i].col.z, 1);
//				glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
//			}
//		}
//	}
//	else
//	{
//		for (int i = 0; i < triangles.size(); i++)
//		{
//			glNormal3f(triangles[i].normal.x, triangles[i].normal.y, triangles[i].normal.z);
//
//			HUtils::XYZ thisCol[3];
//
//			for (int a = 0; a < lights.size(); a++)
//			{
//				for (int c = 0; c < 3; c++)
//				{
//					thisCol[c] = triangles[i].col;
//					float d = pow(lights[a].intensity / fastDist(triangles[i].t[c] + origin, *lights[a].pos), 2);
//					thisCol[c] *= lights[a].col * d;
//				}
//			}
//
//			glColor4f(thisCol[0].x, thisCol[0].y, thisCol[0].z, 1);
//			glVertex3f(triangles[i].t[0].x, triangles[i].t[0].y, triangles[i].t[0].z);
//			glColor4f(thisCol[1].x, thisCol[1].y, thisCol[1].z, 1);
//			glVertex3f(triangles[i].t[1].x, triangles[i].t[1].y, triangles[i].t[1].z);
//			glColor4f(thisCol[2].x, thisCol[2].y, thisCol[2].z, 1);
//			glVertex3f(triangles[i].t[2].x, triangles[i].t[2].y, triangles[i].t[2].z);
//		}
//	}
//	glEnd();
//	glDisable(GL_TEXTURE_2D);
//	glPopMatrix();
//	glDisable(GL_CULL_FACE);
//
//	if (wireFrame == 1 || wireFrame == 3)
//		renderWireFrame();
//}