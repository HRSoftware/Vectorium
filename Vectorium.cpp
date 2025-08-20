
#include "Vectorium.h"

#include <iostream>

#include "include/Engine.h"
#include "UI.h"

int main()
{
	Engine dataEngine;
	UI ui(dataEngine);
	if (!ui.init())
	{
		std::cerr << "Failed to initialize UI\n";
		return -1;
	}

	dataEngine.init();

	while(!ui.shouldClose())
	{
		dataEngine.tick();
		ui.render();
	}

	ui.shutdown();
	dataEngine.shutdown();


	return 0;
}
