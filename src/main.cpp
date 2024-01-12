#include <iostream>

#include "app/SimpleApplication.h"

int main(int argc, char* argv[])
{
	SimpleApplication app;
	Dengine::g_activeApplication = nullptr;
	app.run(argc, argv);
	return 0;
}