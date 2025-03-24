#pragma once

#include <string>

class SceneLoader
{
	static void update();

public:
	static void saveScene(const std::string& path);
	static void loadScene(const std::string& path);

	static void saveSceneDialog();
	static void loadSceneDialog();

	friend class ImGuiHandler;
};
