#include "Variable.h"
#include <iostream>

Variable::Variable(std::string name, float in)
{
	val.first = name;
	val.second = std::to_string(in);
	vType = Variable::valFloat;
}

Variable::Variable(std::string name, int in)
{
	val.first = name;
	val.second = std::to_string(in);
	vType = Variable::valInt;
}

Variable::Variable(std::string name, std::string in)
{
	val.first = name;
	val.second = in;
	vType = Variable::valString;
}

float Variable::getFloat()
{
	if (vType != Variable::valFloat)
		std::cout << val.first << " is not a float.\r\n";

	return atof(val.second.c_str());
}

int Variable::getInt()
{
	if (vType != Variable::valFloat)
		std::cout << val.first << " is not an int.\r\n";

	return atoi(val.second.c_str());
}

std::string Variable::getString()
{
	if (vType != Variable::valFloat)
		std::cout << val.first << " is not a string.\r\n";

	return val.second;
}

std::string Variable::getName()
{
	return val.first;
}

std::string Variable::toString()
{
	return val.second;
}

Variable* Variables::getVar(std::string name)
{
	if (variables.find(name) != variables.end())
	{
		return variables[name];
	}

	std::cout << "Error: no variable found by name: " << name << std::endl;

	return nullptr;
}

Variable* Variables::getVarOrCreate(std::string name, std::string defaultVal, int type)
{
	Variable* r = getVar(name);
	if (r == nullptr)
	{
		r = new Variable(name, defaultVal);
		r->vType = type;
		addVar(r);
	}
	return r;
}

void Variables::setVar(std::string name, std::string newVal, int type)
{
	if (variables.find(name) != variables.end())
	{
		variables[name]->vType = type;
		variables[name]->val.second = newVal;
	}
	else
	{
		std::cout << "Error: no variable found by name: " << name << std::endl;
	}
}

void Variables::addVar(Variable* newVar)
{
	std::cout << "Registered Variable: " << newVar->getName() << ": " << newVar->toString() << std::endl;
	variables[newVar->val.first] = newVar;
}

Variables::variables_t* Variables::getVariables()
{
	return &variables;
}
