#include "Util.h"
#include <iostream>
#include <random>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

glm::vec3 Util::vec3Conv(btVector3 in)
{
	return glm::vec3(in.getX(), in.getY(), in.getZ());
}

btVector3 Util::vec3Conv(glm::vec3 in)
{
	return btVector3(in.x, in.y, in.z);
}

float Util::distanceSquared(glm::vec3 a, glm::vec3 b)
{
	return ((b.x - a.x) * (b.x - a.x)) + ((b.y - a.y) * (b.y - a.y)) + ((b.z - a.z) * (b.z - a.z));
}

float Util::randFloat()
{
	return (float)rand() / RAND_MAX;
}

glm::vec3 Util::randVec()
{
	return glm::normalize(glm::vec3(randFloat() - .5f, randFloat() - .5f, randFloat() - .5f));
}

glm::vec3 Util::vec3FromString(std::string in)
{
	glm::vec3 v = glm::vec3(0.f);
	in += ',';
	int pos = 0;
	std::string temp;
	for (auto& i : in)
	{
		if (i != ' ')
		{
			if (i != ',')
				temp += i;
			else
			{
				if (pos == 0)
					v.x = atof(temp.c_str());
				if (pos == 1)
					v.y = atof(temp.c_str());
				if (pos == 2)
					v.z = atof(temp.c_str());

				pos++;
				temp = "";
			}
		}
	}

	return v;
}

glm::mat4 Util::mat4FromString(std::string in)
{
	float vals[16];
	int pos = 0;
	std::string temp = "";
	for (auto& i : in)
	{
		if (i != ' ')
		{
			if (i != ',')
				temp += i;
			else
			{
				vals[pos] = atof(temp.c_str());
				pos++;
				temp = "";
			}
		}
	}

	glm::mat4 v = glm::make_mat4(vals);
	return v;
}

std::string Util::printVec3(glm::vec3 in)
{
	std::ostringstream o;
	o << in.x << ", " << in.y << ", " << in.z;
	return o.str();
}

std::string Util::printMat4(glm::mat4 in)
{
	std::ostringstream o;
	for (int i = 0; i < 16; ++i)
	{
		o << glm::value_ptr(in)[i] << ", ";
	}
	return o.str();
}
