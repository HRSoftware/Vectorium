
#include "Vectorium.h"

#include <iostream>

#include "include/Engine.h"
#include "UI/UI.h"

int main()
{
	Engine dataEngine;
	dataEngine.init();


	UI ui(dataEngine);
	if (!ui.init())
	{
		std::cerr << "Failed to initialize UI\n";
		return -1;
	}

	while(!ui.shouldClose())
	{
		dataEngine.tick();
		ui.render();
	}

	ui.shutdown();
	dataEngine.shutdown();


	return 0;
}
