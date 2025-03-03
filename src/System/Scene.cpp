#include "Scene.h"

#include "Camera.h"
#include "Graphical.h"
#include "Light.h"
#include "MathExtensions.h"
#include "Model.h"
#include "Pathtracer.h"
#include "SDLHandler.h"
#include "Triangle.h"

void SceneSetup::setupScene()
{
	redGreenRoom();
}

void SceneSetup::museumScene()
{
	auto camera = new Camera({9.09, -4.37, 2.47}, 1, 0, Pathtracer::W_RATIO);
	camera->setRot({0.83f, 0.05f, 0.036f, 0.54f});
	auto tex = Texture::DEFAULT_TEX;

	auto model = Model("models/museum1.obj");
	auto obj = new Mesh({0, 0, 0}, model.triangles);
	obj->setMaterial({Color::white(), true, tex, 1, 0.2f, 2000, 0});
	auto light = new PointLight {{-5.56, -0.19, 14.79}, {255 / 255.0f, 255 / 255.0f, 255.0 / 255.0f}, 1.3f, FLT_MAX};
}

void SceneSetup::churchScene()
{
	auto camera = new Camera({0.30f, -9.57f, 3.03f}, 1, 0, Pathtracer::W_RATIO);
	camera->setRot({0.99f, 0.12f, 0, 0});
	auto tex = Texture::DEFAULT_TEX;

	auto model = Model("models/west.obj");
	auto obj = new Mesh({0, 0, 0}, model.triangles);
	obj->setMaterial({Color::white(), true, tex, 1, 0.2f, 2000, 0});
	auto light = new PointLight {{0.33f, 2.0f, 14.72f}, {255 / 255.0f, 255 / 255.0f, 255 / 255.0f}, 1, FLT_MAX};
}

void SceneSetup::redGreenRoom()
{
	auto camera = new Camera({0, -30, 0}, 1, 0, Pathtracer::W_RATIO);
	auto tex = Texture::DEFAULT_TEX;

	auto light = new PointLight {{0, 0, 8}, {255 / 255.0f, 236 / 255.0f, 156 / 255.0f}, 1, 35};
	auto square = new Square {{0, 0, 9.995f}, 5, {{-90 * DEG_TO_RAD, 0, 0}}};
	square->material()->color = Color::white() * 3;
	auto plane1 = new Square {{-10, 0, 0}, 20, {{0, 0, -90 * DEG_TO_RAD}}};
	auto plane2 = new Square {{10, 0, 0}, 20, {{0, 0, 90 * DEG_TO_RAD}}};
	auto plane3 = new Square {{0, 10, 0}, 20, {{0, 0, 180 * DEG_TO_RAD}}};
	auto plane4 = new Square {{0, 0, -10}, 20, {{90 * DEG_TO_RAD, 0, 0}}};
	auto plane5 = new Square {{0, 0, 10}, 20, {{-90 * DEG_TO_RAD, 0, 0}}};
	auto cube1 = new Cube {glm::vec3 {4, -4, -6.7}, 6.6f, {{0 * DEG_TO_RAD, 0 * DEG_TO_RAD, -28 * DEG_TO_RAD}}};
	auto sphere1 = new Sphere {{-4, 2, -5}, 5};

	auto whiteWallMat = new Material(Color::white(), true, tex, 1, 0, 2000, 0);
	plane1->setMaterial({Color::red(), true, tex, 1, 0, 2000, 0});
	plane2->setMaterial({Color::green(), true, tex, 1, 0, 2000, 0});
	plane3->setSharedMaterial(whiteWallMat);
	plane4->setSharedMaterial(whiteWallMat);
	plane5->setSharedMaterial(whiteWallMat);

	sphere1->setMaterial({Color::skyblue(), true, tex, 1, 0, 2000, 0.3f});
	cube1->setMaterial({Color::skyblue(), true, tex, 1, 0, 2000, 0.3f});
}

void SceneSetup::minecraftHouseScene()
{
	auto camera = new Camera({38.405102, 35.592041, 7.974061}, 1, 0, Pathtracer::W_RATIO);
	camera->setRot({0.38f, {-0.04f, -0.11f, 0.91f}});
	camera->setBackgroundColor({0.1f, 0.1f, 0.1f});
	auto tex = Texture::DEFAULT_TEX;

	auto model = Model("models/minecraft.obj");
	auto obj = new Mesh({0, 0, 0}, model.triangles, {{90 * DEG_TO_RAD, 0, 0}});
	obj->setMaterial({Color::white(), true, tex, 1, 0.2f, 2000, 0});
	auto light = new PointLight {{25, 25, 15}, {255 / 255.0f, 255 / 255.0f, 255 / 255.0f}, 1, FLT_MAX};
}

void SceneSetup::kokuraScene()
{
	auto camera = new Camera({177.24, 173.63, 35.03}, 1, 0, Pathtracer::W_RATIO);
	camera->setRot({0.38f, {0, 0, 0.92f}});
	camera->setBackgroundColor({0.1f, 0.1f, 0.1f});
	auto tex = Texture::DEFAULT_TEX;

	auto model = Model("models/kokura.obj");
	auto obj = new Mesh({0, 0, 0}, model.triangles, {{0, 0, 0}});
	obj->setMaterial({Color::white(), false, tex, 1, 0.2f, 2000, 0});
	auto light = new PointLight {{177.24, 173.63, 100.03}, {255 / 255.0f, 255 / 255.0f, 255 / 255.0f}, 1, FLT_MAX};
}

void SceneSetup::skeletonScene()
{
	auto camera = new Camera({-365.9f, -1406.9f, 508.46f}, 1, 0, Pathtracer::W_RATIO);
	camera->setRot({0.98f, {-0.072f, 0.012f, -0.17f}});
	camera->setBackgroundColor({0.1f, 0.1f, 0.1f});
	auto tex = Texture::DEFAULT_TEX;

	auto model = Model("models/skeleton.obj");
	auto obj = new Mesh({0, 0, 0}, model.triangles);
	obj->setMaterial({Color::white(), true, tex, 1, 0.2f, 2000, 0});
	auto light = new PointLight {{1460.3f, -1246.5f, 423.4f}, {255 / 255.0f, 255 / 255.0f, 255 / 255.0f}, 1, FLT_MAX};
}
