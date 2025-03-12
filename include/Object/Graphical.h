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
	Material* _sharedMaterial = Material::defaultLit;
	Material* _material = nullptr;

protected:
	Graphical(glm::vec3 pos = {}, glm::quat rot = {});
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

class Model
{
	void parse(const std::filesystem::path& path);

public:
	std::vector<Triangle*> triangles;
	std::vector<Vertex> vertices;

	explicit Model(const std::filesystem::path& path);
	void parseRapidobj(const std::filesystem::path& path);
	void parseSelfWritten(const std::filesystem::path& path);
};
