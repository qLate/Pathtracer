#include "Scene.h"

#include "Assets.h"
#include "Camera.h"
#include "Graphical.h"
#include "Light.h"
#include "Model.h"
#include "MyMath.h"
#include "SDLHandler.h"
#include "Triangle.h"
#include "JsonUtility.h"
#include "Renderer.h"
#include "SceneLoader.h"

#ifdef BENCHMARK_BUILD
void SceneSetup::setupScene()
{
	skeletonScene_benchmark();
}
#else
void SceneSetup::setupScene()
{
	redGreenRoom();
	//SceneLoader::loadScene("assets/scenes-pbrt/plants-dusk.pbrt");
}
#endif

void SceneSetup::museumScene()
{
	auto camera = new Camera({9.09, -4.37, 2.47});
	camera->setRot({0.83f, 0.05f, 0.036f, 0.54f});
	auto tex = Texture::defaultTex();

	auto model = Assets::load<Model>("assets/models/museum1.obj");
	auto obj = new Mesh(model);
	obj->setMaterial({Color::white(), true, tex});
	auto light = new PointLight({-5.56, -0.19, 14.79}, {255 / 255.0f, 255 / 255.0f, 255.0 / 255.0f}, 1.3f, FLT_MAX);
}

void SceneSetup::churchScene()
{
	auto camera = new Camera({0.30f, -9.57f, 3.03f});
	camera->setRot({0.99f, 0.12f, 0, 0});

	auto tex = Assets::load<Texture>("assets/textures/church.jpg");

	auto model = Assets::load<Model>("assets/models/west.obj");
	auto obj = new Mesh(model);
	obj->setMaterial({Color::white(), true, tex});
	auto light = new PointLight({0.33f, 2.0f, 14.72f}, {255 / 255.0f, 255 / 255.0f, 255 / 255.0f}, 1, FLT_MAX);
}

void SceneSetup::spiderScene()
{
	auto camera = new Camera({-3.18f, 196.43f, -42.60f});
	camera->setRot({0.0f, {0.0f, -0.052f, -1}});
	camera->setBgColor({0.05f, 0.05f, 0.05f});
	auto tex = Assets::load<Texture>("assets/textures/spider.jpg");

	auto model = Assets::load<Model>("assets/models/spider.obj");
	auto obj = new Mesh(model);
	obj->setMaterial({Color::white(), true, tex});
	auto light = new PointLight({-3.18f, 196.43f, -42.60f}, {255 / 255.0f, 255 / 255.0f, 255 / 255.0f}, 1, FLT_MAX);
}

void SceneSetup::redGreenRoom()
{
	auto camera = new Camera({0, 0, 30});
	camera->setRot(0, 180, 0);

	auto tex = Texture::defaultTex();

	auto square = new Square({0, 9.999f, 0}, 5, {{-180 * DEG_TO_RAD, 0, 0}});
	square->material()->setEmission(Color(255 / 255.0f, 236 * 0.7f / 255.0f, 156 * 0.7f / 255.0f) * 5);

	auto wallLeft = new Square({10, 0, 0}, 20, {{0, 0, 90 * DEG_TO_RAD}});
	auto wallRight = new Square({-10, 0, 0}, 20, {{0, 0, -90 * DEG_TO_RAD}});
	auto wallBack = new Square({0, 0, -10}, 20, {{-90 * DEG_TO_RAD, 0, 0}});
	auto floor = new Square({0, -10, 0}, 20, {{0, 0, 0}});
	auto ceiling = new Square({0, 10, 0}, 20, {{-180 * DEG_TO_RAD, 0, 0}});

	auto cube1 = new Cube({-4, -6.7, 4}, 6.6f, {{0, 28 * DEG_TO_RAD, 0}});
	auto sphere1 = new Sphere({4, -5, -2}, 5);

	auto whiteWallMat = new Material(Color::white(), true, tex);
	wallLeft->setMaterial({Color::red(), true, tex});
	wallRight->setMaterial({Color::green(), true, tex});
	wallBack->setSharedMaterial(whiteWallMat);
	floor->setSharedMaterial(whiteWallMat);
	ceiling->setSharedMaterial(whiteWallMat);

	sphere1->setMaterial({Color::skyblue(), true, tex, 0.001f});
	cube1->setMaterial({Color::skyblue(), true, tex, 1});
}


void SceneSetup::redGreenRoomOriginal()
{
	auto camera = new Camera({278, -800, 278}, 0.69f);
	auto tex = Texture::defaultTex();

	auto white = new Material({0.73f, 0.73f, 0.73f}, true, tex);
	auto red = new Material({0.65f, 0.05f, 0.05f}, true, tex);
	auto green = new Material({0.12f, 0.45f, 0.15f}, true, tex);
	auto lightMat = new Material(Color::white(), true, tex);
	lightMat->setEmission(Color(1.0f, 1.0f, 1.0f) * 15);
	//lightMat->setEmission(Color(1.0f, 0.925f, 0.61f) * 30);

	auto light = new Square({277.5f, 277.5f, 554.5}, 1.0f, {{-90 * DEG_TO_RAD, 0, 0}}); // center first
	light->setScale({130, 1, 105});
	light->setSharedMaterial(lightMat);

	auto leftWall = new Square({0, 277.5f, 277.5f}, 555, {{0, 0, 90 * DEG_TO_RAD}});
	auto rightWall = new Square({555, 277.5f, 277.5f}, 555, {{0, 0, -90 * DEG_TO_RAD}});
	auto floor = new Square({277.5f, 277.5f, 0}, 555, {{90 * DEG_TO_RAD, 0, 0}});
	auto ceiling = new Square({277.5f, 277.5f, 555}, 555, {{-90 * DEG_TO_RAD, 0, 0}});
	auto backWall = new Square({277.5f, 555, 277.5f}, 555, {{0, 0, 180 * DEG_TO_RAD}});

	leftWall->setSharedMaterial(green);
	rightWall->setSharedMaterial(red);
	floor->setSharedMaterial(white);
	ceiling->setSharedMaterial(white);
	backWall->setSharedMaterial(white);

	auto shortBox = new Cube({368, 169, 82.5f}, 165.0f, {{0, 0, -18 * DEG_TO_RAD}});
	shortBox->setMaterial(*white);

	auto tallBox = new Cube({185, 351, 165}, 165.0f, {{0, 0, 15 * DEG_TO_RAD}});
	tallBox->setScale({1, 1, 2});
	tallBox->setMaterial(*white);
}

void SceneSetup::skeletonScene()
{
	auto camera = new Camera({-365.9f * 0.05f, 508.46f * 0.05f, -1406.9f * 0.05f});
	camera->setRot({0.98f, {-0.072f, 0.012f, -0.17f}});
	camera->setBgColor({0.05f, 0.05f, 0.05f});
	auto tex = Texture::defaultTex();

	auto model = Assets::load<Model>("assets/models/skeleton.obj");
	auto obj = new Mesh(model, {}, {{-90 * DEG_TO_RAD, 0, 0}}, {0.05f, 0.05f, 0.05f});
	obj->setMaterial({Color::white(), true, tex, 1});
	auto light = new PointLight({1460.3f, -1246.5f, 423.4f}, {255 / 255.0f, 255 / 255.0f, 255 / 255.0f}, 1, FLT_MAX);
}

void SceneSetup::skeletonScene_benchmark()
{
	Renderer::setSPP(1);
	Renderer::setMaxRayBounces(0);

	auto camera = new Camera({-36.30, 23.65, -43.04});
	camera->setRot({0.933888f, {0.034, 0.355, -0.013}});
	camera->setBgColor({0.0025f, 0.0025f, 0.0025f});
	auto tex = Texture::defaultTex();

	auto model = Assets::load<Model>("assets/models/skeleton.obj");
	auto obj = new Mesh(model, {}, {{-90 * DEG_TO_RAD, 180 * DEG_TO_RAD, 0}}, {0.05f, 0.05f, 0.05f});
	obj->setMaterial({Color::white(), true, tex, 1});
	auto light = new DirectionalLight(normalize(glm::vec3(0, -1, 1)));
	light->setRot({{200.0 * DEG_TO_RAD, -20.08 * DEG_TO_RAD, 14.33 * DEG_TO_RAD}});
}
