// Vectorium.cpp : Defines the entry point for the application.
//

#include "Vectorium.h"

#include "include/Core/IPlugin.h"
#include "include/Core/PluginLoading.h"

using namespace std;



int main()
{
	std::vector<LoadedPlugin> plugins;
	tryLoadPlugin("plugins_bin", plugins);

	for(auto& plugin : plugins)
	{
		std::cout << plugin.getType().value().name() << "\n";
	}

	cout << "Hello" << "\n";

	return 0;
}
