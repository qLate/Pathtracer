#include "Graphical.h"

#include <utility>

#include "Camera.h"
#include "Scene.h"
#include "Triangle.h"
#include "Model.h"

Graphical::Graphical(glm::vec3 pos, glm::quat rot) : Object(pos, rot)

{
	this->indexID = Scene::graphicals.size();
	Scene::graphicals.emplace_back(this);
}

Mesh::Mesh(glm::vec3 pos, std::vector<Triangle*> triangles, glm::quat rot) : Graphical(pos, rot), triangles(std::move(triangles))
{
	for (auto& t : this->triangles)
	{
		t->attachTo(this);
		Scene::triangles.push_back(t);
	}
}
Mesh::~Mesh()
{
	for (const auto& triangle : triangles)
		delete triangle;
}

Square::Square(glm::vec3 pos, float side, glm::quat rot) : Mesh(pos, generateTriangles(side), rot) {}

std::vector<Triangle*> Square::generateTriangles(float side)
{
	auto p1 = glm::vec3(-side / 2, 0, -side / 2);
	auto p2 = glm::vec3(-side / 2, 0, side / 2);
	auto p3 = glm::vec3(side / 2, 0, side / 2);
	auto p4 = glm::vec3(side / 2, 0, -side / 2);

	Vertex vertex1{p1, {0, 0}};
	Vertex vertex2{p2, {1, 0}};
	Vertex vertex3{p3, {1, 1}};
	Vertex vertex4{p4, {0, 1}};

	std::vector<Triangle*> triangles;
	triangles.push_back(new Triangle(this, vertex1, vertex2, vertex3));
	triangles.push_back(new Triangle(this, vertex1, vertex3, vertex4));
	return triangles;
}

Cube::Cube(glm::vec3 pos, float side, glm::quat rot) : Mesh(pos, generateTriangles(side), rot), side(side) {}

std::vector<Triangle*> Cube::generateTriangles(float side)
{
	auto p1 = glm::vec3(-side / 2, -side / 2, -side / 2);
	auto p2 = glm::vec3(-side / 2, -side / 2, side / 2);
	auto p3 = glm::vec3(side / 2, -side / 2, side / 2);
	auto p4 = glm::vec3(side / 2, -side / 2, -side / 2);

	auto p5 = glm::vec3(-side / 2, side / 2, -side / 2);
	auto p6 = glm::vec3(-side / 2, side / 2, side / 2);
	auto p7 = glm::vec3(side / 2, side / 2, side / 2);
	auto p8 = glm::vec3(side / 2, side / 2, -side / 2);

	std::vector<Triangle*> triangles;
	triangles.push_back(new Triangle(this, p1, p3, p2));
	triangles.push_back(new Triangle(this, p1, p4, p3));
	triangles.push_back(new Triangle(this, p5, p6, p7));
	triangles.push_back(new Triangle(this, p5, p7, p8));

	triangles.push_back(new Triangle(this, p1, p2, p6));
	triangles.push_back(new Triangle(this, p1, p6, p5));
	triangles.push_back(new Triangle(this, p4, p7, p3));
	triangles.push_back(new Triangle(this, p4, p8, p7));

	triangles.push_back(new Triangle(this, p2, p3, p7));
	triangles.push_back(new Triangle(this, p2, p7, p6));
	triangles.push_back(new Triangle(this, p1, p8, p4));
	triangles.push_back(new Triangle(this, p1, p5, p8));
	return triangles;
}

Sphere::Sphere(glm::vec3 pos, float radius) : Graphical(pos, {}), radiusSquared(radius * radius), radius(radius) { }

Plane::Plane(glm::vec3 pos, glm::vec3 normal) : Graphical({}, pos), normal{normalize(normal)} { }
