#pragma once

#include <string>

class SceneLoader
{
	inline static bool _isSelectingPath = false;

	static void update();

public:
	static bool isSelectingPath() { return _isSelectingPath; }

	static void saveScene(const std::string& path);

	static void loadScene(const std::string& path);
	static void loadSceneMyFormat(const std::string& path);
	static void loadScenePbrt(const std::string& path);

	static void saveSceneDialog();
	static void loadSceneDialog();

	friend class ImGuiHandler;
};
