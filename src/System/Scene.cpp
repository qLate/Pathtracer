#include "Scene.h"

#include "Camera.h"
#include "Graphical.h"
#include "Light.h"
#include "MathExtensions.h"
#include "Model.h"
#include "Pathtracer.h"
#include "SDLHandler.h"
#include "Triangle.h"

void museumScene()
{
	auto camera = new Camera({ 9.092605, -4.376354, 2.479673 }, 1, 0, glm::vec2((float)Pathtracer::width / (float)Pathtracer::height, 1));
	camera->setRot({ 0.836809f, 0.055576f, 0.036094f, 0.543469f });
	camera->setBackgroundColor(Color::black());
	auto tex = Texture::DEFAULT_TEX;

	auto model = Model("models/museum1.obj");
	auto obj = new Mesh({ 0, 0, 0 }, model.triangles);
	obj->material = new Material(Color::white(), true, tex, 1, 0.2f, 2000, 0);
	auto light = new AreaLight{ {-5.566183, -0.194355, 14.792341}, {255 / 255.0f, 255 / 255.0f, 255.0 / 255.0f}, 1.3f, FLT_MAX, {3, 3, 2}, {6, 6, 1} };
}
void churchScene()
{
	auto camera = new Camera({ 0.305388, -9.574623, 3.030889 }, 1, 0, glm::vec2((float)Pathtracer::width / (float)Pathtracer::height, 1));
	camera->setRot({ 0.992115f, 0.125332f, 0.000000, 0.000004f });
	camera->setBackgroundColor(Color::black());
	auto tex = Texture::DEFAULT_TEX;

	auto model = Model("models/west.obj");
	auto obj = new Mesh({ 0, 0, 0 }, model.triangles);
	obj->material = new Material(Color::white(), true, tex, 1, 0.2f, 2000, 0);
	auto light = new AreaLight{ {0.334557, 2, 14.720142}, {1.0f, 1.0f, 1.0f}, 1, FLT_MAX, {3, 3, 2}, {7, 7, 1} };
}
void redGreenRoom()
{
	auto camera = new Camera({ 0, -30, 0 }, 1, 0, glm::vec2((float)Pathtracer::width / (float)Pathtracer::height, 1));

	auto tex = Texture::DEFAULT_TEX;

	auto light = new AreaLight{ {0, 0, 8}, {255 / 255.0f, 236 / 255.0f, 156 / 255.0f}, 1, 25, {0, 0, 0}, {1, 1, 1} };
	auto square = new Square{ {0, 0, 9.995f}, 5, {{-90 * DEG_TO_RAD, 0, 0}} };
	square->material->color = { 300.f, 300.f, 300.f };
	auto plane1 = new Square{ {-10, 0, 0}, 20, {{0, 0, -90 * DEG_TO_RAD}} };
	auto plane2 = new Square{ {10, 0, 0}, 20, {{0, 0, 90 * DEG_TO_RAD}} };
	auto plane3 = new Square{ {0, 10, 0}, 20, {{0, 0, 180 * DEG_TO_RAD}} };
	auto plane4 = new Square{ {0, 0, -10}, 20, {{90 * DEG_TO_RAD, 0, 0}} };
	auto plane5 = new Square{ {0, 0, 10}, 20, {{-90 * DEG_TO_RAD, 0, 0}} };
	auto cube1 = new Cube{ glm::vec3 {4, -4, -6.7}, 6.6f, {{0 * DEG_TO_RAD, 0 * DEG_TO_RAD, -28 * DEG_TO_RAD}} };
	auto sphere1 = new Sphere{ {-4, 2, -5}, 5 };

	auto whiteWallMat = new Material(Color::white(), true, tex, 1, 0, 2000, 0);
	plane1->material = new Material(Color::red(), true, tex, 1, 0, 2000, 0);
	plane2->material = new Material(Color::green(), true, tex, 1, 0, 2000, 0);
	plane3->material = whiteWallMat;
	plane4->material = whiteWallMat;
	plane5->material = whiteWallMat;

	sphere1->material = new Material(Color::skyblue(), true, tex, 1, 0, 2000, 0.3f);
	cube1->material = new Material(Color::skyblue(), true, tex, 1, 0, 2000, 0.3f);
}
void minecraftHouseScene()
{
	auto camera = new Camera({ 38.405102, 35.592041, 7.974061 }, 1, 0, glm::vec2((float)Pathtracer::width / (float)Pathtracer::height, 1));
	camera->setRot({ 0.387786f, {-0.047615, -0.112185, 0.913657} });
	camera->setBackgroundColor({ 0.1f, 0.1f, 0.1f });
	auto tex = Texture::DEFAULT_TEX;

	auto model = Model("models/minecraft.obj");
	auto obj = new Mesh({ 0, 0, 0 }, model.triangles, { {90 * DEG_TO_RAD, 0, 0} });
	obj->material = new Material(Color::white(), true, tex, 1, 0.2f, 2000, 0);
	auto light = new AreaLight{ {25, 25, 15}, {1.0f, 1.0f, 1.0f}, 1, FLT_MAX, {5, 5, 5}, {6, 6, 6} };
}
void kokuraScene()
{
	auto camera = new Camera({ 177.245255, 173.638443, 35.038555 }, 1, 0, glm::vec2((float)Pathtracer::width / (float)Pathtracer::height, 1));
	camera->setRot({ 0.380992f, {0.006647, 0.016129, 0.924414} });
	camera->setBackgroundColor({ 0.1f, 0.1f, 0.1f });
	auto tex = Texture::DEFAULT_TEX;

	auto model = Model("models/kokura.obj");
	auto obj = new Mesh({ 0, 0, 0 }, model.triangles, { {0, 0, 0} });
	obj->material = new Material(Color::white(), false, tex, 1, 0.2f, 2000, 0);
	auto light = new AreaLight{ {177.245255, 173.638443, 100.038555}, {1.0f, 1.0f, 1.0f}, 1, FLT_MAX, {5, 5, 5}, {6, 6, 1} };
}

void SceneSetup::setupScene()
{
	redGreenRoom();
}
