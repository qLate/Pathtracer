#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "Object.h"
#include "Material.h"
#include "Triangle.h"

class BVHNode;
class AABB;
class Model;

class Graphical : public Object
{
	int _indexId;

	Material* _sharedMaterial = Material::defaultLit();
	Material* _material = nullptr;

protected:
	Graphical(glm::vec3 pos = {}, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	~Graphical() override;

public:
	int indexID() const { return _indexId; }

	Material* material();
	Material* materialNoCopy() const;
	Material* sharedMaterial() const;

	void setMaterial(const Material& material);
	void setSharedMaterial(Material* material);
};


class Mesh : public Graphical
{
	std::vector<Triangle*> _triangles {};

public:
	Mesh(std::vector<Triangle*> triangles, glm::vec3 pos = {}, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	Mesh(const Model& model, glm::vec3 pos = {}, glm::quat rot = {}, glm::vec3 scale = { 1, 1, 1 });
	~Mesh() override;

	std::vector<Triangle*> triangles() const { return _triangles; }

	void setPos(glm::vec3 pos, bool notify = true) override;
	void setRot(glm::quat rot, bool notify = true) override;
	void setScale(glm::vec3 scale, bool notify = true) override;
};


class Square : public Mesh
{
public:
	Square(glm::vec3 pos, float side, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	std::vector<Triangle*> generateTriangles(float side);
};


class Cube final : public Mesh
{
	float _side;

public:
	Cube(glm::vec3 pos, float side, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	std::vector<Triangle*> generateTriangles(float side);

	float side() const { return _side; }
};


class Sphere final : public Graphical
{
	float _radius;

public:
	Sphere(glm::vec3 pos, float radius, glm::vec3 scale = {1, 1, 1});

	float radius() const { return _radius; }
	void setRadius(float radius) { _radius = radius; }
};


class Plane final : public Graphical
{
	glm::vec3 _normal;

public:
	Plane(glm::vec3 pos, glm::vec3 normal);

	glm::vec3 normal() const { return _normal; }
	void setNormal(glm::vec3 normal) { _normal = normal; }
};
