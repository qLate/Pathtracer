#include "SceneLoader.h"

#include <vector>

#include "Camera.h"
#include "Graphical.h"
#include "ImFileDialog.h"
#include "Light.h"
#include "Scene.h"
#include "Model.h"


void SceneLoader::update()
{
	if (ifd::FileDialog::Instance().IsDone("SaveScene"))
	{
		if (ifd::FileDialog::Instance().HasResult())
		{
			auto path = ifd::FileDialog::Instance().GetResult().string();
			saveScene(path);
		}
		ifd::FileDialog::Instance().Close();
	}

	if (ifd::FileDialog::Instance().IsDone("OpenScene"))
	{
		if (ifd::FileDialog::Instance().HasResult())
		{
			auto path = ifd::FileDialog::Instance().GetResult().string();
			loadScene(path);
		}
		ifd::FileDialog::Instance().Close();
	}
}

void SceneLoader::saveScene(const std::string& path)
{
	nlohmann::json json;

	std::vector<nlohmann::json> serializedObjects;
	for (auto obj : Scene::objects)
	{
		nlohmann::json objJson;

		if (auto camera = dynamic_cast<Camera*>(obj))
			objJson = JsonUtility::toJson(*camera);
		else if (auto cube = dynamic_cast<Cube*>(obj))
			objJson = JsonUtility::toJson(*cube);
		else if (auto square = dynamic_cast<Square*>(obj))
			objJson = JsonUtility::toJson(*square);
		else if (auto mesh = dynamic_cast<Mesh*>(obj))
			objJson = JsonUtility::toJson(*mesh);
		else if (auto sphere = dynamic_cast<Sphere*>(obj))
			objJson = JsonUtility::toJson(*sphere);
		else if (auto pointLight = dynamic_cast<PointLight*>(obj))
			objJson = JsonUtility::toJson(*pointLight);
		else if (auto dirLight = dynamic_cast<DirectionalLight*>(obj))
			objJson = JsonUtility::toJson(*dirLight);

		serializedObjects.push_back(objJson);
	}

	json["objects"] = serializedObjects;

	std::ofstream file(path);
	file << json.dump(4);
	file.close();
}
void SceneLoader::loadScene(const std::string& path)
{
	std::ifstream file(path);
	nlohmann::json json;
	file >> json;
	file.close();

	for (const auto& objJson : json["objects"])
	{
		std::string type = objJson["classType"];
		if (type == "Camera")
			new Camera(JsonUtility::fromJson<Camera>(objJson));
		else if (type == "Mesh")
			new Mesh(JsonUtility::fromJson<Mesh>(objJson));
		else if (type == "Cube")
			new Cube(JsonUtility::fromJson<Cube>(objJson));
		else if (type == "Square")
			new Square(JsonUtility::fromJson<Square>(objJson));
		else if (type == "Sphere")
			new Sphere(JsonUtility::fromJson<Sphere>(objJson));
		else if (type == "PointLight")
			new PointLight(JsonUtility::fromJson<PointLight>(objJson));
		else if (type == "DirectionalLight")
			new DirectionalLight(JsonUtility::fromJson<DirectionalLight>(objJson));
	}
}
void SceneLoader::saveSceneDialog()
{
	auto dir = std::filesystem::current_path().concat("/assets/scenes/").string();
	ifd::FileDialog::Instance().Open("SaveScene", "Save a scene file", "Scene file (*.json){.json},.*", true, dir);
}
void SceneLoader::loadSceneDialog()
{
	auto dir = std::filesystem::current_path().concat("/assets/scenes/").string();
	ifd::FileDialog::Instance().Open("OpenScene", "Open a scene file", "Scene file (*.json){.json},.*", false, dir);
}
