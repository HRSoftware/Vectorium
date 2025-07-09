
#include "Vectorium.h"

#include "include/DataPacket/DataPacketRegistry.h"
#include "include/Plugin/IPlugin.h"
#include "include/Plugin/PluginInstance.h"
#include "include/Plugin/PluginManager.h"
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
