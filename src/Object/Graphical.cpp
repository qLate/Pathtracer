#include "Graphical.h"

#include <fstream>

#include "Camera.h"
#include "Scene.h"
#include "Triangle.h"

#include "rapidobj.hpp"
#include "Utils.h"


Graphical::Graphical(glm::vec3 pos, glm::quat rot, glm::vec3 scale) : Object(pos, rot, scale)
{
	this->indexID = Scene::graphicals.size();
	Scene::graphicals.emplace_back(this);
}
Graphical::~Graphical()
{
	delete _material;
}

Material* Graphical::material()
{
	if (_material) return _material;

	_material = new Material(*_sharedMaterial);
	return _material;
}
Material* Graphical::materialNoCopy() const
{
	if (_material) return _material;
	return _sharedMaterial;
}
Material* Graphical::sharedMaterial() const
{
	return _sharedMaterial;
}

void Graphical::setMaterial(const Material& material)
{
	delete _material;
	this->_material = new Material(material);
}
void Graphical::setSharedMaterial(Material* material)
{
	if (_material)
	{
		delete _material;
		_material = nullptr;
	}

	_sharedMaterial = material;
}

Mesh::Mesh(std::vector<Triangle*> triangles, glm::vec3 pos, glm::quat rot, glm::vec3 scale) : Graphical(pos, rot, scale), triangles(std::move(triangles))
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
void Mesh::setPos(glm::vec3 pos)
{
	Graphical::setPos(pos);
	for (auto& t : triangles)
		t->updateGeometry();
}
void Mesh::setRot(glm::quat rot)
{
	Graphical::setRot(rot);
	for (auto& t : triangles)
		t->updateGeometry();
}
void Mesh::setScale(glm::vec3 scale)
{
	Graphical::setScale(scale);
	for (auto& t : triangles)
		t->updateGeometry();
}

Square::Square(glm::vec3 pos, float side, glm::quat rot, glm::vec3 scale) : Mesh(generateTriangles(side), pos, rot, scale) {}
std::vector<Triangle*> Square::generateTriangles(float side)
{
	auto p1 = glm::vec3(-side / 2, 0, -side / 2);
	auto p2 = glm::vec3(-side / 2, 0, side / 2);
	auto p3 = glm::vec3(side / 2, 0, side / 2);
	auto p4 = glm::vec3(side / 2, 0, -side / 2);

	Vertex vertex1 {p1, {0, 0}};
	Vertex vertex2 {p2, {1, 0}};
	Vertex vertex3 {p3, {1, 1}};
	Vertex vertex4 {p4, {0, 1}};

	std::vector<Triangle*> triangles;
	triangles.push_back(new Triangle(vertex1, vertex2, vertex3, this));
	triangles.push_back(new Triangle(vertex1, vertex3, vertex4, this));
	return triangles;
}

Cube::Cube(glm::vec3 pos, float side, glm::quat rot, glm::vec3 scale) : Mesh(generateTriangles(side), pos, rot, scale), side(side) {}
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
	triangles.push_back(new Triangle(p1, p3, p2, this));
	triangles.push_back(new Triangle(p1, p4, p3, this));
	triangles.push_back(new Triangle(p5, p6, p7, this));
	triangles.push_back(new Triangle(p5, p7, p8, this));

	triangles.push_back(new Triangle(p1, p2, p6, this));
	triangles.push_back(new Triangle(p1, p6, p5, this));
	triangles.push_back(new Triangle(p4, p7, p3, this));
	triangles.push_back(new Triangle(p4, p8, p7, this));

	triangles.push_back(new Triangle(p2, p3, p7, this));
	triangles.push_back(new Triangle(p2, p7, p6, this));
	triangles.push_back(new Triangle(p1, p8, p4, this));
	triangles.push_back(new Triangle(p1, p5, p8, this));
	return triangles;
}

Sphere::Sphere(glm::vec3 pos, float radius, glm::vec3 scale) : Graphical(pos, {}, scale), radius(radius) {}

Plane::Plane(glm::vec3 pos, glm::vec3 normal) : Graphical({}, pos), normal {normalize(normal)} {}
