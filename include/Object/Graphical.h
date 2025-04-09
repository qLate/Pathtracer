#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "Object.h"
#include "Material.h"
#include "Model.h"
#include "Triangle.h"

class BVHNode;
class AABB;
class Model;

class Graphical : public Object
{
	int _indexId = -1;

protected:
	Material* _sharedMaterial = Material::defaultLit();
	Material* _material = nullptr;

	Graphical(glm::vec3 pos, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	Graphical(const Graphical& other);
	Graphical() = default;
	void init();

	~Graphical() override;

public:
	int indexId() const { return _indexId; }

	Material* material();
	Material* materialNoCopy() const;
	Material* sharedMaterial() const;

	void setScale(glm::vec3 scale, bool notify = true) override;

	void setMaterial(const Material& material);
	void setSharedMaterial(Material* material);

	constexpr static auto properties();

private:
	Graphical* clone_internal() const override { return new Graphical(*this); }
	void drawInspector() override { return GraphicalInspectorDrawer::draw(this); }
};


class Mesh : public Graphical
{
protected:
	Model* _model;

	Mesh() = default;

	void init(Model* model);

public:
	Mesh(Model* model, glm::vec3 pos = {}, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	Mesh(const Mesh& orig);

	Model* model() const { return _model; }

	void setModel(Model* model);

	constexpr static auto properties();

private:
	Mesh* clone_internal() const override { return new Mesh(*this); }
	void drawInspector() override { return MeshInspectorDrawer::draw(this); }

	friend class JsonUtility;
};


class Square final : public Mesh
{
	float _side;

	Square();

public:
	Square(glm::vec3 pos, float side, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	Square(const Square& orig);
	static Model* getBaseModel();

	float side() const { return _side; }

	constexpr static auto properties();

private:
	Square* clone_internal() const override { return new Square(*this); }

	friend class JsonUtility;
};


class Cube final : public Mesh
{
	float _side;

	Cube();

public:
	Cube(glm::vec3 pos, float side, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	Cube(const Cube& orig);
	static Model* getBaseModel();

	float side() const { return _side; }

	constexpr static auto properties();

private:
	Cube* clone_internal() const override { return new Cube(*this); }

	friend class JsonUtility;
};


class Sphere final : public Graphical
{
	float _radius;

	Sphere() = default;

public:
	Sphere(glm::vec3 pos, float radius, glm::vec3 scale = {1, 1, 1});
	Sphere(const Sphere& orig);

	float radius() const { return _radius; }
	void setRadius(float radius);

	constexpr static auto properties();

private:
	Sphere* clone_internal() const override { return new Sphere(*this); }
	void drawInspector() override { return SphereInspectorDrawer::draw(this); }

	friend class JsonUtility;
};


class Plane final : public Graphical
{
	Plane() = default;

public:
	Plane(glm::vec3 pos, glm::vec3 rot = {});
	Plane(const Plane& orig);

	constexpr static auto properties();

private:
	Plane* clone_internal() const override { return new Plane(*this); }

	friend class JsonUtility;
};


constexpr auto Graphical::properties()
{
	return std::tuple_cat(
		Object::properties(),
		std::make_tuple(
			JsonUtility::property(&Graphical::_sharedMaterial, "sharedMaterial"),
			JsonUtility::property(&Graphical::_material, "material")
		)
	);
}

constexpr auto Mesh::properties()
{
	return std::tuple_cat(
		Graphical::properties(),
		std::make_tuple(
			JsonUtility::property(&Mesh::_model, "model")
		)
	);
}

constexpr auto Square::properties()
{
	return std::tuple_cat(
		Graphical::properties(),
		std::make_tuple(
			JsonUtility::property(&Square::_side, "side")
		)
	);
}

constexpr auto Cube::properties()
{
	return std::tuple_cat(
		Graphical::properties(),
		std::make_tuple(
			JsonUtility::property(&Cube::_side, "side")
		)
	);
}

constexpr auto Sphere::properties()
{
	return std::tuple_cat(
		Graphical::properties(),
		std::make_tuple(
			JsonUtility::property(&Sphere::_radius, "radius")
		)
	);
}

constexpr auto Plane::properties()
{
	return std::tuple_cat(
		Graphical::properties()
	);
}
