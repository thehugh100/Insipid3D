#include "Console.h"
#include "engine.h"
#include <GLFW/glfw3.h>
#include "Variable.h"

Console::Console(Engine* enginePtr)
	:engine(enginePtr)
{
	consoleShowing = 0;
	commandHistoryIndex = 0;

	commands["reloadShaders"] = [this](std::string params)
	{
		this->engine->shaderManager->flushShaders();
		return "done.";
	};

	commands["help"] = [this](std::string params)
	{
		std::string r = "";
		for (auto& i : this->commands)
			r += i.first + "\n";

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

	size_t spacePos = command.find(" ");
	std::string commandName = command.substr(0, spacePos);
	std::string parameters = "";
	int hasParameters = 0;
	if (spacePos != -1)
	{
		parameters = command.substr(spacePos + 1);
		hasParameters = 1;
	}

	if (commands.find(command) != commands.end())
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
	}
	if (consoleShowing)
	{
		if (engine->input->keyPressed(GLFW_KEY_ENTER))
		{
			consolePrint(">> " + engine->input->keyboardBuffer);

			parseCommand(engine->input->keyboardBuffer);

			engine->input->keyboardBuffer = "";
		}
		if (engine->input->keyPressed(GLFW_KEY_UP))
		{
			if (commandHistory.size() > 0)
			{
				commandHistoryIndex--;
				engine->input->keyboardBuffer = commandHistory[commandHistoryIndex % commandHistory.size()];
			}
		}
		if (engine->input->keyPressed(GLFW_KEY_DOWN))
		{
			if (commandHistory.size() > 0)
			{
				commandHistoryIndex++;
				engine->input->keyboardBuffer = commandHistory[commandHistoryIndex % commandHistory.size()];
			}
		}
		if (engine->input->keyPressed(GLFW_KEY_TAB))
		{
			std::string keyboardBuf = engine->input->keyboardBuffer;
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
			for (auto& i : posibilities)
			{
				if (i.find(keyboardBuf) == 0)
					engine->input->keyboardBuffer = i;
			}
		}
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

		for (int i = glm::max((int)consoleBuffer.size()- fontSize, 0); i < consoleBuffer.size(); i++)
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
