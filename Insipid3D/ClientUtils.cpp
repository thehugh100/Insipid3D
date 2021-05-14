#include "ClientUtils.h"

#include <iostream>
#include <sstream>

bool checkJSON(nlohmann::json j, std::string fieldName)
{
	return !j.empty() && j.contains(fieldName);
}

bool readJSONAsString(nlohmann::json j, std::string fieldName, std::string& field, bool debugPrint)
{
	if (debugPrint)
	{
		std::cout << "Attempting field name " << fieldName << std::endl;
		std::cout << "JSON " << j.dump(4) << std::endl;
	}
	if (checkJSON(j, fieldName))
	{
		if (j.at(fieldName).is_string())
			field = j.at(fieldName).get<std::string>();
		else
			field = j.at(fieldName).dump();

		if (debugPrint)
			std::cout << "JSON value found: '" << field << "'" << std::endl;
		return true;
	}
	else
	{
		if (debugPrint)
			std::cout << "JSON doesnt contain field name " << fieldName << std::endl;
	}
	return false;
}




Args extractLiteralArgs(std::string input)
{
	std::stringstream ss(input);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	Args args(begin, end);
	Args newArgs;
	std::string tempArg;
	bool midArg = false;
	for (auto arg : args)
	{
		if (!midArg && !arg.empty() && (arg[0] == '"'))
		{
			tempArg.clear();
			arg.erase(0, 1);
			tempArg += arg + " ";
			if (arg[arg.size() - 1] != '"')
				midArg = true;
		}
		else if (midArg && !arg.empty()
			&& (arg[arg.size() - 1] == '"'))
		{
			arg.erase(arg.size() - 1, 1);
			tempArg += arg;
			midArg = false;
			newArgs.push_back(tempArg);
		}
		else if (midArg)
		{
			tempArg += arg + " ";
		}
		else
		{
			newArgs.push_back(arg);
		}
	}

	return newArgs;
}