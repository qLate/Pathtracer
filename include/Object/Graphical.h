#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "Object.h"
#include "Material.h"
#include "Triangle.h"

class BVHNode;
class AABB;

class Graphical : public Object
{
	Material* _sharedMaterial = Material::defaultLit();
	Material* _material = nullptr;

protected:
	Graphical(glm::vec3 pos = {}, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	~Graphical() override;

public:
	int indexID;

	Material* material();
	Material* materialNoCopy() const;
	Material* sharedMaterial() const;

	void setMaterial(const Material& material);
	void setSharedMaterial(Material* material);
};


class Mesh : public Graphical
{
public:
	std::vector<Triangle*> triangles {};

	Mesh(std::vector<Triangle*> triangles, glm::vec3 pos = {}, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	~Mesh() override;

	void setPos(glm::vec3 pos) override;
	void setRot(glm::quat rot) override;
	void setScale(glm::vec3 scale) override;
};


class Square : public Mesh
{
public:
	Square(glm::vec3 pos, float side, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	std::vector<Triangle*> generateTriangles(float side);
};


class Cube final : public Mesh
{
public:
	float side;

	Cube(glm::vec3 pos, float side, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	std::vector<Triangle*> generateTriangles(float side);
};


class Sphere final : public Graphical
{
public:
	float radius;

	Sphere(glm::vec3 pos, float radius, glm::vec3 scale = {1, 1, 1});
};


class Plane final : public Graphical
{
public:
	glm::vec3 normal;

	Plane(glm::vec3 pos, glm::vec3 normal);
};
