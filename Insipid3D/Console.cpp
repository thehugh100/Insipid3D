#include "Console.h"
#include "engine.h"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <fstream>
#include "Variable.h"
#include "map.h"

Console::Console(Engine* enginePtr)
	:engine(enginePtr)
{
	consoleShowing = 0;
	commandHistoryIndex = 0;
	lastAutocompleteIndex = -1;
	preAutocompleteInputBuf = "";
	consoleScroll = 0;

	consoleBuffer = { "-- INSIPID 3D --", "Type: 'help' for a list of commands."};

	commands["reloadShaders"] = [this](std::string params)
	{
		this->engine->shaderManager->flushShaders();
		return "done.";
	};

	commands["generateLightmap"] = [this](std::string params)
	{
		engine->map->generateLightmap();
		return "done.";
	};

	commands["loadMap"] = [this](std::string params)
	{
		std::string mapPath = "worlds/" + params;
		if (!std::experimental::filesystem::exists(mapPath))
		{
			return std::string("Could not find map: " + mapPath);
		}

		Map *nMap = new Map(mapPath, engine->textureManager);

		if (nMap->map->scene != nullptr)
			engine->map = nMap;

		return  std::string("done.");
	};

	commands["help"] = [this](std::string params)
	{
		std::string r = "";
		for (auto& i : this->commands)
			r += i.first + "\n";

		return r;
	};

	commands["debugVariables"] = [this](std::string params)
	{
		std::string r = "";
		for (auto& i : *engine->variables->getVariables())
			r += i.first + ": " + i.second->toString() + "\n";

		return r;
	};

	commands["listEntities"] = [this](std::string params)
	{
		std::string r = "";
		EntityList e;
		engine->entityManger->getAllEntities(&e);

		int index = 0;

		for (auto& i : e)
		{
			std::string traits = "";
			int tIndex = 0;
			for (auto& t : *i->entityTraits.getTraits())
			{
				traits += t;
				if (tIndex < i->entityTraits.getTraits()->size() - 1)
					traits += ", ";

				tIndex++;
			}

			r += std::to_string(index) + ": (" + traits + ")\n";

			index++;
		}

		return r;
	};

	commands["exit"] = [this](std::string params)
	{
		exit(0);
		return "";
	};

	commands["fps"] = [this](std::string params)
	{
		return std::to_string(engine->fps);
	};
}

void Console::parseCommand(std::string command)
{
	commandHistory.push_back(command);
	commandHistoryIndex = commandHistory.size();
	consoleLines = 16;

	size_t spacePos = command.find(" ");
	std::string commandName = command.substr(0, spacePos);
	std::string parameters = "";
	int hasParameters = 0;
	if (spacePos != -1)
	{
		parameters = command.substr(spacePos + 1);
		hasParameters = 1;
	}

	if (commands.find(commandName) != commands.end())
	{
		consolePrint(commands[commandName](parameters));
	}
	else
	{
		Variable* v = engine->variables->getVar(commandName);
		if (v != nullptr)
		{
			if (!hasParameters)
			{
				consolePrint(v->toString());
			}
			else
			{
				engine->variables->setVar(v->getName(), parameters, v->vType);
			}
		}
		else
		{
			consolePrint("No such command " + command);
		}
	}
}

void Console::tick()
{
	if (engine->input->keyPressed(GLFW_KEY_GRAVE_ACCENT))
	{
		engine->input->keyboardBuffer = "";
		consoleShowing = !consoleShowing;
		lastAutocompleteIndex = -1;
	}
	if (consoleShowing)
	{
		if (engine->input->keyPressed(GLFW_KEY_ENTER))
		{
			lastAutocompleteIndex = -1;
			consolePrint(">> " + engine->input->keyboardBuffer);
			parseCommand(engine->input->keyboardBuffer);
			engine->input->keyboardBuffer = "";
		}
		if (engine->input->keyPressed(GLFW_KEY_UP))
		{
			if (commandHistory.size() > 0)
			{
				lastAutocompleteIndex = -1;
				commandHistoryIndex--;
				engine->input->keyboardBuffer = commandHistory[commandHistoryIndex % commandHistory.size()];
			}
		}
		if (engine->input->keyPressed(GLFW_KEY_DOWN))
		{
			if (commandHistory.size() > 0)
			{
				lastAutocompleteIndex = -1;
				commandHistoryIndex++;
				engine->input->keyboardBuffer = commandHistory[commandHistoryIndex % commandHistory.size()];
			}
		}
		if (engine->input->keyPressed(GLFW_KEY_TAB))
		{
			std::string keyboardBuf = engine->input->keyboardBuffer;
			preAutocompleteInputBuf = keyboardBuf;

			std::vector<std::string> posibilities;

			Variables::variables_t variables = *engine->variables->getVariables();
			for (auto& i : variables)
			{
				posibilities.push_back(i.first);
			}
			for (auto& i : commands)
			{
				posibilities.push_back(i.first);
			}
			int idx = 0;
			for (auto& i : posibilities)
			{
				if (i.find(preAutocompleteInputBuf) == 0 && idx > lastAutocompleteIndex)
				{
					engine->input->keyboardBuffer = i;
					lastAutocompleteIndex = idx;
				}
				idx++;
			}
		}
		consoleLines = engine->variables->getVarOrCreate("consoleHeightLines", "16", Variable::valInt)->getInt();
		consoleScroll += engine->input->scrollOffsetY;
		consoleScroll = glm::clamp(consoleScroll, 0, glm::max((int)consoleBuffer.size() - consoleLines, 0));
	}
}

void Console::render()
{
	if (consoleShowing)
	{
		std::string consoleFont = "fonts/Roboto_Mono/RobotoMono-Regular.ttf";
		int fontSize = 18;

		bool showCursor = ((int)engine->getElapsedTimeMS() / 250) % 2 == 0;
		char cursorVal[2] = {' ', '_'};

		float yOffset = engine->screen.y - fontSize;

		glDepthMask(GL_FALSE);

		int historyStartIdx = glm::max((int)consoleBuffer.size() - consoleLines - consoleScroll, 0);
		int historyEndIdx = glm::min(consoleBuffer.size() - consoleScroll, consoleBuffer.size());

		historyEndIdx = glm::clamp(historyEndIdx, 0, (int)consoleBuffer.size());
		historyStartIdx = glm::clamp(historyStartIdx, 0, (int)consoleBuffer.size());

		for (int i = historyStartIdx; i < historyEndIdx; i++)
		{
			engine->drawDebugText(engine->fontManager->getFont(consoleFont, fontSize), 2, yOffset, consoleBuffer[i], glm::vec3(0.f));
			engine->drawDebugText(engine->fontManager->getFont(consoleFont, fontSize), 3, yOffset - 1, consoleBuffer[i]);
			yOffset -= fontSize;
		}
		
		engine->drawDebugText(engine->fontManager->getFont(consoleFont, fontSize), 2, yOffset, engine->input->keyboardBuffer + cursorVal[showCursor], glm::vec3(0));
		engine->drawDebugText(engine->fontManager->getFont(consoleFont, fontSize), 3, yOffset - 1, engine->input->keyboardBuffer + cursorVal[showCursor]);
		glDepthMask(GL_TRUE);
	}
}

void Console::consolePrint(std::string message)
{
	std::cout << message << std::endl;

	std::string temp = "";
	for (auto& i : message)
	{
		if (i == '\n')
		{
			consoleBuffer.push_back(temp);
			temp = "";
		}
		else
			temp += i;
	}
	consoleBuffer.push_back(temp);
}
