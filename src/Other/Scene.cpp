#include "Scene.h"

#include "Camera.h"
#include "Graphical.h"
#include "Light.h"
#include "Model.h"
#include "MyMath.h"
#include "SDLHandler.h"
#include "Triangle.h"

void SceneSetup::setupScene()
{
	skeletonScene();
}

void SceneSetup::museumScene()
{
	auto camera = new Camera({9.09, -4.37, 2.47});
	camera->setRot({0.83f, 0.05f, 0.036f, 0.54f});
	auto tex = Texture::defaultTex();

	auto model = Model("assets/models/museum1.obj");
	auto obj = new Mesh(model);
	obj->setMaterial({Color::white(), true, tex});
	auto light = new PointLight({-5.56, -0.19, 14.79}, {255 / 255.0f, 255 / 255.0f, 255.0 / 255.0f}, 1.3f, FLT_MAX);
}

void SceneSetup::churchScene()
{
	auto camera = new Camera({0.30f, -9.57f, 3.03f});
	camera->setRot({0.99f, 0.12f, 0, 0});
	//auto camera = new Camera({ 7.494653, 8.752432, 11.397732 });
	//camera->setRot({ 0.963874, {0.189301, -0.036111, -0.183870} });
	//camera->setBgColor(Color::white());
	auto tex = new Texture("assets/textures/church.jpg");

	auto model = Model("assets/models/west.obj");
	auto obj = new Mesh(model);
	obj->setMaterial({Color::white(), true, tex});
	auto light = new PointLight({0.33f, 2.0f, 14.72f}, {255 / 255.0f, 255 / 255.0f, 255 / 255.0f}, 1, FLT_MAX);
}

void SceneSetup::spiderScene()
{
	auto camera = new Camera({-3.18f, 196.43f, -42.60f});
	camera->setRot({0.0f, {0.0f, -0.052f, -1}});
	camera->setBgColor({0.05f, 0.05f, 0.05f});
	auto tex = new Texture("assets/textures/spider.jpg");

	auto model = Model("assets/models/spider.obj");
	auto obj = new Mesh(model);
	obj->setMaterial({Color::white(), true, tex});
	auto light = new PointLight({-3.18f, 196.43f, -42.60f}, {255 / 255.0f, 255 / 255.0f, 255 / 255.0f}, 1, FLT_MAX);
}

void SceneSetup::redGreenRoom()
{
	auto camera = new Camera({0, -30, 0});
	//camera->setBackgroundColor(Color::white());
	auto tex = new Texture("assets/textures/marble.jpg");

	auto light = new PointLight({0, 0, 8}, {255 / 255.0f, 236 / 255.0f, 156 / 255.0f}, 1, 35);
	auto square = new Square({0, 0, 9.995f}, 5, {{-90 * DEG_TO_RAD, 0, 0}});
	square->material()->setColor(Color::white() * 3);
	auto plane1 = new Square({-10, 0, 0}, 20, {{0, 0, -90 * DEG_TO_RAD}});
	auto plane2 = new Square({10, 0, 0}, 20, {{0, 0, 90 * DEG_TO_RAD}});
	auto plane3 = new Square({0, 10, 0}, 20, {{0, 0, 180 * DEG_TO_RAD}});
	auto plane4 = new Square({0, 0, -10}, 20, {{90 * DEG_TO_RAD, 0, 0}});
	auto plane5 = new Square({0, 0, 10}, 20, {{-90 * DEG_TO_RAD, 0, 0}});
	auto cube1 = new Cube(glm::vec3 {4, -4, -6.7}, 6.6f, {{0 * DEG_TO_RAD, 0 * DEG_TO_RAD, -28 * DEG_TO_RAD}});
	auto sphere1 = new Sphere({-4, 2, -5}, 5);

	auto whiteWallMat = new Material(Color::white(), true, tex);
	plane1->setMaterial({Color::red(), true, tex});
	plane2->setMaterial({Color::green(), true, tex});
	plane3->setSharedMaterial(whiteWallMat);
	plane4->setSharedMaterial(whiteWallMat);
	plane5->setSharedMaterial(whiteWallMat);

	sphere1->setMaterial({Color::skyblue(), true, tex, 1, 0.3f});
	cube1->setMaterial({Color::skyblue(), true, tex, 1, 0.3f});
}

void SceneSetup::minecraftHouseScene()
{
	auto camera = new Camera({38.40, 35.59, 7.97});
	camera->setRot({0.38f, {-0.04f, -0.11f, 0.91f}});
	camera->setBgColor({0.05f, 0.05f, 0.05f});
	auto tex = Texture::defaultTex();

	auto model = Model("assets/models/minecraft.obj");
	auto obj = new Mesh(model, {}, {{90 * DEG_TO_RAD, 0, 0}});
	obj->setMaterial({Color::white(), true, tex, 1});
	auto light = new PointLight({25, 25, 15}, {255 / 255.0f, 255 / 255.0f, 255 / 255.0f}, 1, FLT_MAX);
}

void SceneSetup::kokuraScene()
{
	auto camera = new Camera({177.24, 173.63, 35.03});
	camera->setRot({0.38f, {0, 0, 0.92f}});
	camera->setBgColor({0.05f, 0.05f, 0.05f});
	auto tex = Texture::defaultTex();

	auto model = Model("assets/models/kokura.obj");
	auto obj = new Mesh(model);
	obj->setMaterial({Color::white(), false, tex, 1});
	auto light = new PointLight {{177.24, 173.63, 100.03}, {255 / 255.0f, 255 / 255.0f, 255 / 255.0f}, 1, FLT_MAX};
}

void SceneSetup::skeletonScene()
{
	auto camera = new Camera({-365.9f * 0.05f, -1406.9f * 0.05f, 508.46f * 0.05f});
	camera->setRot({0.98f, {-0.072f, 0.012f, -0.17f}});
	camera->setBgColor({0.05f, 0.05f, 0.05f});
	auto tex = Texture::defaultTex();

	auto model = Model("assets/models/skeleton.obj");
	auto obj = new Mesh(model, {}, {}, {0.05f, 0.05f, 0.05f});
	obj->setMaterial({Color::white(), true, tex, 1});
	auto light = new PointLight({1460.3f, -1246.5f, 423.4f}, {255 / 255.0f, 255 / 255.0f, 255 / 255.0f}, 1, FLT_MAX);
}

void SceneSetup::spaceStationScene()
{
	auto camera = new Camera({-34.931564, 44.459042, 47.477470});
	camera->setRot({0.28f, {-0.065754, 0.213367, -0.932158}});
	camera->setBgColor({0.05f, 0.05f, 0.05f});
	auto tex = Texture::defaultTex();

	auto model = Model("assets/models/spaceStationScene/Space Station Scene.obj");
	auto obj = new Mesh(model, {}, glm::quat({90, 0, 0}));
	obj->setMaterial({Color::white(), true, tex, 1});
	auto light = new PointLight({-3.19f, 44.20f, 54.34f}, {255 / 255.0f, 255 / 255.0f, 255 / 255.0f}, 1, FLT_MAX);
}
