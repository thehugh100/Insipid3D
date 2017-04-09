#include <iostream>
#include <windows.h>
#include "GL\freeglut.h"
#include "globals.h"
#include "render.h"
#include "engine.h"
using namespace std;

int main(int argc, char **argv)
{
	engine = new CEngine(argc, argv);
	engine->run();
    return 0;
}