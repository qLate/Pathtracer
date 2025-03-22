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
protected:
	int _indexId;

	Material* _sharedMaterial = Material::defaultLit();
	Material* _material = nullptr;

	Graphical(glm::vec3 pos = {}, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	Graphical(const Graphical& other);
	void init();
	~Graphical() override;

public:
	int indexId() const { return _indexId; }

	Material* material();
	Material* materialNoCopy() const;
	Material* sharedMaterial() const;

	void setMaterial(const Material& material);
	void setSharedMaterial(Material* material);

private:
	Graphical* clone_internal() const override { return new Graphical(*this); }
	void drawInspector() override { return GraphicalInspectorDrawer::draw(this); }
};


class Mesh : public Graphical
{
protected:
	Model* _model;
	std::vector<Triangle*> _triangles;

	void init(const Model* model);

public:
	Mesh(Model* model, glm::vec3 pos = {}, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	Mesh(const Mesh& orig);
	~Mesh() override;

	std::vector<Triangle*> triangles() const;

private:
	Mesh* clone_internal() const override { return new Mesh(*this); }
};


class Square : public Mesh
{
	float _side;

public:
	Square(glm::vec3 pos, float side, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	Square(const Square& orig);
	static Model* getBaseModel();

	float side() const { return _side; }

private:
	Square* clone_internal() const override { return new Square(*this); }
};


class Cube final : public Mesh
{
	float _side;

public:
	Cube(glm::vec3 pos, float side, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	Cube(const Cube& orig);
	static Model* getBaseModel();

	float side() const { return _side; }

private:
	Cube* clone_internal() const override { return new Cube(*this); }
};


class Sphere final : public Graphical
{
	float _radius;

public:
	Sphere(glm::vec3 pos, float radius, glm::vec3 scale = {1, 1, 1});
	Sphere(const Sphere& orig);

	float radius() const { return _radius; }
	void setRadius(float radius) { _radius = radius; }

private:
	Sphere* clone_internal() const override { return new Sphere(*this); }
};


class Plane final : public Graphical
{
	glm::vec3 _normal;

public:
	Plane(glm::vec3 pos, glm::vec3 normal);
	Plane(const Plane& orig);

	glm::vec3 normal() const { return _normal; }
	void setNormal(glm::vec3 normal) { _normal = normal; }

private:
	Plane* clone_internal() const override { return new Plane(*this); }
};
