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

	static void loadScene_objects(const minipbrt::Scene* scene, const std::vector<Material*>& materials);
	static std::vector<BaseTriangle*> loadModelTriangles(const minipbrt::TriangleMesh* mesh);
	static std::vector<Model*> loadScene_objects_loadModels(const minipbrt::Scene* scene);
	static Graphical* spawnObjectFromShape(const minipbrt::Shape* shape, const std::vector<Material*>& materials, Model* model);

	static void loadScene_lights(minipbrt::Scene* scene);

public:
	static void loadScene(const std::string& path);
};
