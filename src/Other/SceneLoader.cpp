#include "SceneLoader.h"

#include <vector>

#include "Camera.h"
#include "Graphical.h"
#include "ImFileDialog.h"
#include "Light.h"
#include "Scene.h"
#include "Model.h"
#include "ObjectManipulator.h"


void SceneLoader::update()
{
	if (ifd::FileDialog::Instance().IsDone("SaveScene"))
	{
		_isSelectingPath = false;

		if (ifd::FileDialog::Instance().HasResult())
		{
			auto path = ifd::FileDialog::Instance().GetResult().string();
			saveScene(path);
		}
		ifd::FileDialog::Instance().Close();
	}

	if (ifd::FileDialog::Instance().IsDone("OpenScene"))
	{
		_isSelectingPath = false;

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
		if (obj == nullptr) continue;
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
		else if (auto plane = dynamic_cast<Plane*>(obj))
			objJson = JsonUtility::toJson(*plane);
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
	for (int i = Scene::objects.size() - 1; i >= 0; i--)
		delete Scene::objects[i];
	Scene::objects.clear();

	//for (int i = Scene::materials.size() - 1; i >= 0; i--)
	//	delete Scene::materials[i];
	//Scene::materials.clear();

	//for (int i = Scene::textures.size() - 1; i >= 0; i--)
	//	delete Scene::textures[i];
	//Scene::textures.clear();

	ObjectManipulator::deselectObject();

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
		else if (type == "Plane")
			new Plane(JsonUtility::fromJson<Plane>(objJson));
		else if (type == "DirectionalLight")
			new DirectionalLight(JsonUtility::fromJson<DirectionalLight>(objJson));
	}

	BufferController::updateLights();
}
void SceneLoader::saveSceneDialog()
{
	_isSelectingPath = true;

	auto dir = std::filesystem::current_path().concat("/assets/scenes/").string();
	ifd::FileDialog::Instance().Save("SaveScene", "Save a scene file", "Scene file (*.json){.json},.*", dir);
}
void SceneLoader::loadSceneDialog()
{
	_isSelectingPath = true;

	auto dir = std::filesystem::current_path().concat("/assets/scenes/").string();
	ifd::FileDialog::Instance().Open("OpenScene", "Open a scene file", "Scene file (*.json){.json},.*", false, dir);
}
