#pragma once

#include <string>

#include "Graphical.h"
#include "Material.h"
#include "minipbrt.h"

class SceneLoaderPbrt
{
	static void loadScene_camera(minipbrt::Scene* scene);
	static void loadScene_textures(const minipbrt::Scene* scene, std::vector<Texture*>& parsedTextures);
	static void loadScene_materials(const minipbrt::Scene* scene, const std::vector<Texture*>& parsedTextures, std::vector<Material*>& materials);

	static void loadScene_shapes(const minipbrt::Scene* scene, const std::vector<Material*>& materials);
	static Object* spawnObjectFromShape(minipbrt::Shape* shape, const std::vector<Material*>& materials);
	static Mesh* spawnObjectFromShape_triMesh(const std::vector<Material*>& materials, const minipbrt::Shape* shape, std::mutex& mutex);

	static void loadScene_lights(minipbrt::Scene* scene);

public:
	static void loadScene(const std::string& path);
};
