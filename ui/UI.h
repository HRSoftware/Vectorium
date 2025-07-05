#pragma once
#include "EngineUIBridge.h"
#include "GLFW/glfw3.h"

class Engine;

class UI
{
public:
	UI(Engine& engine);
	bool init();
	void render() const;
	bool shouldClose() const;
	void shutdown();

	private:
		std::unique_ptr<EngineUIBridge> uiBridge;
		Engine& engine;
		GLFWwindow* window = nullptr;
};