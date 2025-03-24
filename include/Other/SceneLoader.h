#pragma once

#include <string>

class SceneLoader
{
public:
	static void saveScene(const std::string& path);
	static void loadScene(const std::string& path);
};
