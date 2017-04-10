/*
Hugh Nixon
17/12/2016

Last edited 10/04/2017
Simple Header that does some usefull keyoard / mouse / game engine stuff...
*/
#pragma once
#include <iostream>
#include <Windows.h>
#include <thread>
#include <time.h>
#include <vector>
#include <sstream>
#include <map>
#include <math.h>
#include <minmax.h>
using namespace std;
namespace HUtils
{
	void keyboardThread();
	void detachKeyboardHandler();
	void mouseThread();
	void detatchMouseHandler();

	struct XYZ
	{
		XYZ(float nx, float ny, float nz)
		{
			x = nx;
			y = ny;
			z = nz;
		}
		XYZ()
		{
			x = 0;
			y = 0;
			z = 0;
		}

		XYZ &operator * (XYZ a)
		{
			return XYZ(a.x * x, a.y * y, a.z * z);
		}
		XYZ &operator / (float a)
		{
			return XYZ(x / a, y / a, z / a);
		}
		XYZ &operator + (XYZ a)
		{
			return XYZ(a.x + x, a.y + y, a.z + z);
		}
		XYZ &operator - (XYZ a)
		{
			return XYZ(a.x - x, a.y - y, a.z - z);
		}
		XYZ &operator += (XYZ a)
		{
			x += a.x;
			y += a.y;
			z += a.z;
			return *this;
		}
		XYZ &operator *= (XYZ a)
		{
			x *= a.x;
			y *= a.y;
			z *= a.z;
			return *this;
		}
		XYZ &operator * (float a)
		{
			return XYZ(a * x, a * y, a * z);
		}
		XYZ &operator + (float a)
		{
			return XYZ(a + x, a + y, a + z);
		}
		XYZ &operator *= (float a)
		{
			x *= a;
			y *= a;
			z *= a;
			return *this;
		}
		inline void normalize()
		{
			float m = sqrt(x * x + y * y + z * z);
			x /= m;
			y /= m;
			z /= m;
		}
		inline XYZ normalized()
		{
			float m = sqrt(x * x + y * y + z * z);
			return XYZ(x / m, y / m, z / m);
		}
		inline float magnitude(XYZ in)
		{
			return sqrt(in.x * in.x + in.y * in.y + in.z * in.z);
		}
		inline float magnitude()
		{
			return sqrt(x * x + y * y + z * z);
		}
		void print()
		{
			std::cout << "X: " << x << " Y: " << y << " Z: " << z << endl;
		}
		string getValues()
		{
			ostringstream q;
			q << "X: " << x << " Y: " << y << " Z: " << z;
			return q.str();
		}
		void print(string title)
		{
			std::cout << title.c_str() << " = X: " << x << " Y: " << y << " Z: " << z << endl;
		}
		float x, y, z;
	};
	inline float distance2XYZ(XYZ p1, XYZ p2)
	{
		return sqrtf(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2) + pow(p2.z - p1.z, 2));
	}
	inline float fastDist(XYZ a, XYZ b)
	{
		return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z);
	}
	float Dot(XYZ v1, XYZ v2)
	{
		float p1 = (v1.x * v2.x ) + (v1.y * v2.y) + (v1.z * v2.z);
		return p1;
	}
	/*Rotates A normalized vector*/
	XYZ yRotate(XYZ in, float ang)
	{
		ang = ang * 3.1415 / 180.0f;
		float nx = in.x * cos(ang) - in.z * sin(ang);
		float ny = in.x * sin(ang) + in.z * cos(ang);
		return XYZ(nx, 0, ny);
	}
	XYZ randVec()
	{
		return XYZ(((rand() % 100000) / 100000.0) - ((rand() % 100000) / 100000.0), ((rand() % 100000) / 100000.0) - ((rand() % 100000) / 100000.0), ((rand() % 100000) / 100000.0) - ((rand() % 100000) / 100000.0));
	}
	struct CGlobals_
	{
		string gWindowName;
	};

	float clamp(float in, float lowerBound, float upperBound)
	{
		return max(lowerBound, min(upperBound, in));
	}

	CGlobals_ global;

	unsigned char keyList[][3];

	struct CKeyboardController
	{
		char oldKeys[256];
		char newKeys[256];

		int focusOnMyWindow;
		string windowName;
		HWND win;

		CKeyboardController()
		{
			for (int i = 0; i < 256; i++)
			{
				oldKeys[i] = 0;
				newKeys[i] = 0;
			}
			focusOnMyWindow = 0;
			win = 0;
			windowName = "";
		}

		void run()
		{
			if (focusOnMyWindow)
			{
				if (GetForegroundWindow() != win)
				{
					for (int i = 0; i < 256; i++)
					{
						oldKeys[i] = 0;
						newKeys[i] = 0;
					}
					return;
				}
			}

			for (int i = 0; i < 256; i++)
			{
				oldKeys[i] = newKeys[i];
				newKeys[i] = !!GetAsyncKeyState(i);
			}
		}
		/*Run the key update but don't check which window has focus*/
		void runOverrideWindow()
		{
			for (int i = 0; i < 256; i++)
			{
				oldKeys[i] = newKeys[i];
				newKeys[i] = !!GetAsyncKeyState(i);
			}
		}

		void setFocusWindow(string _windowName)
		{
			windowName = _windowName;
			win = FindWindowA(NULL, _windowName.c_str());
			focusOnMyWindow = 1;
		}

		bool keyDown(int key)
		{
			return newKeys[key];
		}
		bool keyDownEvent(int key)
		{
			return newKeys[key] && !oldKeys[key];
		}
		void detach()
		{
			detachKeyboardHandler();
		}
		char getChar()
		{
			for (int i = 0; i < 48; ++i)
				if (keyDownEvent(keyList[i][0]))
					if (keyDown(VK_SHIFT))
						return keyList[i][2];
					else
						return keyList[i][1];
			return -1;
		}
	};
	CKeyboardController *keyboard = new CKeyboardController();

	void keyboardThread()
	{
		while (1)
		{
			keyboard->run();
			Sleep(8);
		}
	}
	void detachKeyboardHandler()
	{
		thread proc(keyboardThread);
		proc.detach();
	}

	struct CMouseController
	{
		int x;
		int y;
		int usingWindow;
		string windowName;
		HWND win;
		int offsetX;
		int offsetY;

		int oldLeftValue;
		int leftValue;
		int oldRightValue;
		int rightValue;
		int indate;

		int focusOnMyWindow;

		int leftDown()
		{
			return leftValue;
		}
		int rightDown()
		{
			return rightValue;
		}

		int leftDownEvent()
		{
			return 	leftValue && !oldLeftValue;
		}
		int rightDownEvent()
		{
			return rightValue && !oldRightValue;
		}
		int leftUpEvent()
		{
			return !leftValue && oldLeftValue;
		}
		int rightUpEvent()
		{
			return !rightValue && oldRightValue;
		}

		CMouseController()
		{
			x = 0;
			y = 0;
			usingWindow = 0;
			windowName = "";
			offsetX = 0;
			offsetY = 0;
			win = 0;
			leftValue = 0;
			oldLeftValue = 0;
			rightValue = 0;
			oldRightValue = 0;
			indate = 0;
			focusOnMyWindow = 0;
		}

		void setFocusWindow(string _windowName)
		{
			windowName = _windowName;
			win = FindWindowA(NULL, windowName.c_str());
			focusOnMyWindow = 1;
		}

		void setRelativeWindow(string _windowName)
		{
			usingWindow = 1;
			windowName = _windowName;
			win = FindWindowA(NULL, windowName.c_str());
		}
		void run()
		{
			if (focusOnMyWindow)
			{
				if (GetForegroundWindow() != win)
				{
					leftValue = 0;
					rightValue = 0;
					oldLeftValue = 0;
					oldRightValue = 0;
					return;
				}
			}

			POINT p;
			GetCursorPos(&p);
			int tx = p.x;
			int ty = p.y;

			if (usingWindow)
			{
				ScreenToClient(win, &p);
				tx = p.x;
				ty = p.y;
			}
			x = tx;
			y = ty;

			oldLeftValue = leftValue;
			leftValue = !!GetAsyncKeyState(VK_LBUTTON);

			oldRightValue = rightValue;
			rightValue = !!GetAsyncKeyState(VK_RBUTTON);

		}
		void detach()
		{
			detatchMouseHandler();
		}
	};
	CMouseController *mouse = new CMouseController();

	void mouseThread()
	{
		while (1)
		{
			mouse->run();
			Sleep(8);
		}
	}
	void detatchMouseHandler()
	{
		thread mouseHandler(mouseThread);
		mouseHandler.detach();
	}

	void setWindowTitle(string nWindowName)
	{
		global.gWindowName = nWindowName;
	}

	bool windowHasFocus()
	{
		return GetForegroundWindow() == FindWindowA(NULL, global.gWindowName.c_str());
	}

	unsigned char keyList[][3] =
	{
		{ 0x41, 'a', 'A' },
		{ 0x42, 'b', 'B' },
		{ 0x43, 'c', 'C' },
		{ 0x44, 'd', 'D' },
		{ 0x45, 'e', 'E' },
		{ 0x46, 'f', 'F' },
		{ 0x47, 'g', 'G' },
		{ 0x48, 'h', 'H' },
		{ 0x49, 'i', 'I' },
		{ 0x4A, 'j', 'J' },
		{ 0x4B, 'k', 'K' },
		{ 0x4C, 'l', 'L' },
		{ 0x4D, 'm', 'M' },
		{ 0x4E, 'n', 'N' },
		{ 0x4F, 'o', 'O' },
		{ 0x50, 'p', 'P' },
		{ 0x51, 'q', 'Q' },
		{ 0x52, 'r', 'R' },
		{ 0x53, 's', 'S' },
		{ 0x54, 't', 'T' },
		{ 0x55, 'u', 'U' },
		{ 0x56, 'v', 'V' },
		{ 0x57, 'w', 'W' },
		{ 0x58, 'x', 'X' },
		{ 0x59, 'y', 'Y' },
		{ 0x5A, 'z', 'Z' },
		{ 0x30, '0', ')' },
		{ 0x31, '1', '!' },
		{ 0x32, '2', '"' },
		{ 0x33, '3', '£' },
		{ 0x34, '4', '$' },
		{ 0x35, '5', '%' },
		{ 0x36, '6', '^' },
		{ 0x37, '7', '&' },
		{ 0x38, '8', '*' },
		{ 0x39, '9', '(' },
		{ 0xDC, '\\', '|' },
		{ 0xBC, ',', '<' },
		{ 0xBE, '.', '>' },
		{ 0xBF, '/', '?' },
		{ 0xBA, ';', ':' },
		{ 0xC0, '\'', '@' },
		{ 0xDE, '#', '~' },
		{ 0xBD, '-', '_' },
		{ 0xBB, '=', '+' },
		{ 0xDB, '[', '{' },
		{ 0xDD, ']', '}' },
		{ 0x20, ' ', ' ' }
	};

}

void(*conCall)(string);
bool hasConsoleCallback = 0;
map<string, string> vmVariables;
int gLine = 0;
struct CScriptEngine
{
	struct CScriptEngine_Func
	{
		CScriptEngine_Func(string funcName_, int parameterCount_, string(*functionPointer_)(vector<string>))
		{
			funcName = funcName_;
			functionPointer = functionPointer_;
			parameterCount = parameterCount_;
		}
		CScriptEngine_Func()
		{

		}
		string funcName;
		string(*functionPointer)(vector<string> params); //function pointer must be static!
		int parameterCount;
	};
	vector <CScriptEngine_Func> registeredFunctions;

	struct StandardLib
	{
		static string exec(string cmd)
		{
			FILE* pipe = _popen(cmd.c_str(), "r");
			if (!pipe) return "ERROR";
			char buffer[128];
			std::string result = "";
			while (!feof(pipe)) {
				if (fgets(buffer, 128, pipe) != NULL)
					result += buffer;
			}
			_pclose(pipe);
			return result;
		}

		static string vCmd(vector<string> params)
		{
			return exec(params[0]);
		}

		static string vMsgbox(vector<string> params)
		{
			return to_string(MessageBoxA((HWND)atoi(params[0].c_str()), params[1].c_str(), params[2].c_str(), atoi(params[3].c_str())));
		}

		static string vSleep(vector<string> params)
		{
			Sleep(atoi(params[0].c_str()));
			return "";
		}

		static string setVal(vector<string> params)
		{
			vmVariables[params[0]] = params[1];
			return params[1];
		}
		static string getVal(vector<string> params)
		{
			return vmVariables[params[0]];
		}
		static string _echo(vector<string> params)
		{
			string built = "";
			for (int i = 0; i < params.size(); i++)
				built += params[i];

			if (hasConsoleCallback)
			{
				conCall(built);
			}
			return built;
		}
		static string concat(vector<string> params)
		{
			string built = "";
			for (int i = 0; i < params.size(); i++)
				built += params[i];
			return built;
		}
		static string _input(vector<string> params)
		{
			string val = "";
			getline(cin, val);
			return val;
		}
		static string _goto(vector<string> params)
		{
			gLine = atoi(params[0].c_str());
			return "";
		}
		static string _endl(vector<string> params)
		{
			return "\r\n";
		}
		static string _quote(vector<string> params)
		{
			return "\"";
		}
		static string _exit(vector<string> params)
		{
			exit(0);
			return "";
		}
		static string _getLocalPath(vector<string> params)
		{
			char q[256];
			GetCurrentDirectoryA(256, q);
			return string(q);
		}

		static string _createProcess(vector<string> params)
		{
			STARTUPINFOA info = { sizeof(info) };
			PROCESS_INFORMATION processInfo;
			CreateProcessA(params[0].c_str(), (LPSTR)params[1].c_str(), 0, 0, 0, 0, 0, 0, &info, &processInfo);
			return to_string(processInfo.dwProcessId);
		}
		void load(CScriptEngine *p)
		{
			p->registerFunction(CScriptEngine_Func("cmd", 1, &vCmd));
			p->registerFunction(CScriptEngine_Func("messageBox", 4, &vMsgbox));
			p->registerFunction(CScriptEngine_Func("sleep", 1, &vSleep));
			p->registerFunction(CScriptEngine_Func("setVal", 2, &setVal));
			p->registerFunction(CScriptEngine_Func("getVal", 1, &getVal));
			p->registerFunction(CScriptEngine_Func("goto", 1, &_goto));
			p->registerFunction(CScriptEngine_Func("echo", 0, &_echo));
			p->registerFunction(CScriptEngine_Func("concat", 0, &concat));
			p->registerFunction(CScriptEngine_Func("input", 0, &_input));
			p->registerFunction(CScriptEngine_Func("endl", 0, &_endl));
			p->registerFunction(CScriptEngine_Func("quote", 0, &_quote));
			p->registerFunction(CScriptEngine_Func("exit", 0, &_exit));
			p->registerFunction(CScriptEngine_Func("getLocalPath", 0, &_getLocalPath));
			p->registerFunction(CScriptEngine_Func("createProcess", 2, &_createProcess));
			//p->registerFunction(CScriptEngine_Func("getRegisteredFuncs", 0, &getRegisteredFuncs));
		}
	};
	StandardLib* stdlib = new StandardLib;

	void registerFunction(CScriptEngine_Func func)
	{
		registeredFunctions.push_back(func);
	}
	CScriptEngine()
	{
		stdlib->load(this);
	}
	string stripSpaces(string in)
	{
		string o = "";
		int inQuotes = 0;
		for (int i = 0; i < in.length(); i++)
		{
			if (in[i] == '"')
				inQuotes = !inQuotes;
			if (!inQuotes)
			{
				if (in[i] != ' ')
				{
					o += in[i];
				}
			}
			else
			{
				o += in[i];
			}
		}
		return o;
	}
	string decodeParameter(string param)
	{
		if (param[0] == '"')
		{
			//string
			string toFilter = param.substr(1, param.length() - 2);
			string filtered = "";
			int hitEscapedSeq = 0;
			for (int i = 0; i < toFilter.length(); i++)
			{
				if (toFilter[i] == '\\')
				{
					hitEscapedSeq = !hitEscapedSeq;
				}
				if (!hitEscapedSeq)
				{
					filtered += toFilter[i];
				}
			}
			return filtered;
		}
		else
		{
			//test for variables here
			{
				//function?
				if (param.find("(") != -1 && param.find(")") != -1) //broken logic... could be inside string
				{
					return processLine(param);
				}
				else
				{
					return param;
				}
			}
		}
		return "0";
	}

	string callFunction(string functionName, vector<string> parameters)
	{
		for (int i = 0; i < parameters.size(); i++)
		{
			parameters[i] = decodeParameter(parameters[i]);
		}

		for (int i = 0; i < registeredFunctions.size(); i++)
		{
			if (functionName == registeredFunctions[i].funcName)
			{
				if (registeredFunctions[i].parameterCount != parameters.size() && registeredFunctions[i].parameterCount != 0)
				{
					return "Incorrect parameter count in \"" + functionName + "\": Expected " + to_string(registeredFunctions[i].parameterCount) + " parameters, got " + to_string(parameters.size());
				}
				return registeredFunctions[i].functionPointer(parameters);
			}
		}

		return "";
	}

	string processLine(string script)
	{
		if (script.find("(") != -1 && script.find(")") != -1)
		{
			string normalized = "";
			string functionName = "";
			vector<string> parameters;

			normalized = stripSpaces(script);

			functionName = normalized.substr(0, normalized.find("("));

			string tempStr = "";
			int ignoreCommas = 0;
			int inQuote = 0;
			for (int i = normalized.find_first_of("(") + 1; i < normalized.find_last_of(")") + 1; i++)
			{

				if (normalized[i] == '"')
					inQuote = !inQuote;

				if (!inQuote)
				{
					if (normalized[i] == '(')
						ignoreCommas++;
					if (normalized[i] == ')')
						ignoreCommas--;
				}
				if ((normalized[i] == ',' && !ignoreCommas && !inQuote) || i == normalized.find_last_of(")"))
				{
					parameters.push_back(tempStr);
					tempStr = "";
				}
				else
				{
					tempStr += normalized[i];
				}
			}
			return callFunction(functionName, parameters);
		}
		else
		{
			/*vairables here*/
		}
		return "";
	}
	void runScript(string script_)
	{
		std::thread scriptThread(&CScriptEngine::detachedScriptHandler, this, script_);
		scriptThread.detach();
	}

	string runScriptSR(string script)
	{
		//consoleOutputRelay("->'" + processLine(script) + "'");
		//break script into lines
		vector<string> lines;
		string tempLine = "";
		int inQuote = 0;
		for (int i = 0; i < script.length(); i++)
		{
			if (script[i] == '"')
				inQuote = !inQuote;

			if (script[i] != ';')
			{
				if (script[i] != '\r' && script[i] != '\n')
					tempLine += script[i];

			}
			else
			{
				if (!inQuote)
				{
					if (tempLine[0] != '#')
						lines.push_back(tempLine);
					tempLine = "";
				}
			}

		}
		string finalOut = "";
		for (gLine = 0; gLine < lines.size(); gLine++)
		{
			finalOut += processLine(lines[gLine]);
		}
		return finalOut;
	}

	void detachedScriptHandler(string script)
	{
		SetConsoleTitleA(runScriptSR(script).c_str());
		//if (hasConsoleCallback)
		//{
		//	conCall(finalOut);
		//}
	}

	void registerConsoleCallback(void(*call)(string))
	{
		hasConsoleCallback = 1;
		conCall = call;
	}
};
CScriptEngine *script = new CScriptEngine();