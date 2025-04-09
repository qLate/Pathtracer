#pragma once

#include <vector>

class Model;
class BaseTriangle;
class Material;
class Light;
class Object;
class Graphical;
class Triangle;
class Texture;

class Scene
{
public:
	inline static std::vector<Object*> objects{};
	inline static std::vector<Light*> lights{};
	inline static std::vector<Graphical*> graphicals{};
	inline static std::vector<BaseTriangle*> baseTriangles{};

	inline static std::vector<Material*> materials{};
	inline static std::vector<Texture*> textures{};
	inline static std::vector<Model*> models{};
};

class SceneSetup
{
	static void setupScene();

	static void emptyScene();
	static void churchScene();
	static void spiderScene();
	static void redGreenRoom();
	static void redGreenRoomOriginal();
	static void skeletonScene();
	static void skeletonScene_benchmark();

	friend class Program;
};
