#pragma once

#include <nlohmann/json.hpp>

extern bool checkJSON(nlohmann::json j, std::string fieldName);
template <typename TYPE>
inline bool readJSON(nlohmann::json j, std::string fieldName, TYPE& field, bool debugPrint = false)
{
	if (checkJSON(j, fieldName))
	{
		field = j.at(fieldName).get<TYPE>();
		if (debugPrint)
			std::cout << "DEGUB read: " << fieldName << " = " << field << std::endl;
		return true;
	}
	else
	{
		if (debugPrint)
			std::cout << "Failed to read: " << fieldName << std::endl;
	}
	return false;
}

extern bool readJSONAsString(nlohmann::json j, std::string fieldName, std::string& field, bool debugPrint = false);

/**
	Create a new scope when getting a string from a json field.
	json_get_string(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL)
	>	std::string DATA_SYMBOL;
	>	if (readJSONAsString(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL))
*/
#define json_get_string(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL) \
	std::string DATA_CONTAINER_SYMBOL; \
	if (readJSONAsString(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL))
/**
	Create a new scope when getting a json object from a json field.
	json_get_object(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL)
	>	nlohmann::json DATA_CONTAINER_SYMBOL;
	>	if (readJSON(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL))
*/
#define json_get_object(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL) \
	if (readJSON(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL))


typedef std::function<void(std::string)> ClientCommand;
typedef std::vector<std::string> Args;


extern Args extractLiteralArgs(std::string input);