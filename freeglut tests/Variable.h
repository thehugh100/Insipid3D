#pragma once
#include <string>
#include <map>
#include <algorithm>

struct Variable
{
	Variable(std::string name, float in);
	Variable(std::string name, int in);
	Variable(std::string name, std::string in);

	float getFloat();
	int getInt();
	std::string getString();
	std::string getName();
	std::string toString();
	enum valType {valFloat, valInt, valString};

	int vType;
	std::pair<std::string, std::string> val;
	std::string defaultVal;
};

class Variables
{
public:
	Variable* getVar(std::string name);
	Variable* getVarOrCreate(std::string name, std::string defaultVal, int type);
	void setVar(std::string name, std::string newVal, int type);
	void addVar(Variable* newVar);
	
	typedef std::map<std::string, Variable*> variables_t;
	variables_t* getVariables();
private:
	variables_t variables;
};