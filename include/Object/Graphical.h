#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "Object.h"
#include "Material.h"

class BVHNode;
class AABB;
class Triangle;

class Graphical : public Object
{
protected:
	Graphical(glm::vec3 pos = {}, glm::quat rot = {});

public:
	int indexID;
	Material* material = Material::DEFAULT_LIT;
};


class Mesh : public Graphical
{
public:
	std::vector<Triangle*> triangles {};

	Mesh(glm::vec3 pos, std::vector<Triangle*> triangles, glm::quat rot = {});
	~Mesh() override;
};


class Square : public Mesh
{
public:
	Square(glm::vec3 pos, float side, glm::quat rot = {});
	std::vector<Triangle*> generateTriangles(float side);
};


class Cube final : public Mesh
{
public:
	float side;

	Cube(glm::vec3 pos, float side, glm::quat rot);
	std::vector<Triangle*> generateTriangles(float side);
};


class Sphere final : public Graphical
{
public:
	float radius;

	Sphere(glm::vec3 pos, float radius);
};


class Plane final : public Graphical
{
public:
	glm::vec3 normal;

	Plane(glm::vec3 pos, glm::vec3 normal);
};
