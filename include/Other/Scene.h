#pragma once

#include <vector>

class Material;
class Light;
class Object;
class Graphical;
class Triangle;
class Texture;

class Scene
{
public:
	inline static std::vector<Object*> objects {};
	inline static std::vector<Light*> lights {};
	inline static std::vector<Graphical*> graphicals {};
	inline static std::vector<Triangle*> triangles {};

	inline static std::vector<Material*> materials {};
	inline static std::vector<Texture*> textures {};
};

class SceneSetup
{
	static void setupScene();

	static void museumScene();
	static void churchScene();
	static void spiderScene();
	static void redGreenRoom();
	static void redGreenRoomOriginal();
	static void minecraftHouseScene();
	static void kokuraScene();
	static void skeletonScene();
	static void skeletonScene_benchmark();
	static void spaceStationScene();

	friend class Program;
};
