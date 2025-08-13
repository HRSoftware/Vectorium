
#include "Vectorium.h"
#include "ui/UI.h"
#include "include/Engine.h"

int main()
{
	Engine dataEngine;
	dataEngine.init();

	UI ui(dataEngine);
	ui.init();

	while(!ui.shouldClose())
	{
		dataEngine.tick();
		ui.render();
	}

	ui.shutdown();
	dataEngine.shutdown();


	return 0;
}
