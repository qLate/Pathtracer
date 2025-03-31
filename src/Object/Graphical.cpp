#include "Graphical.h"

#include "BufferController.h"
#include "Camera.h"
#include "Scene.h"
#include "Triangle.h"

#include "Utils.h"
#include "Model.h"

Graphical::Graphical(glm::vec3 pos, glm::quat rot, glm::vec3 scale) : Object(pos, rot, scale)
{
	init();
}
Graphical::Graphical(const Graphical& other) : Object(other), _sharedMaterial(other.materialNoCopy())
{
	init();
}
void Graphical::init()
{
	_indexId = Scene::graphicals.size();
	Scene::graphicals.emplace_back(this);
}

Graphical::~Graphical()
{
	if (!initialized()) return;

	auto ind = std::ranges::find(Scene::graphicals, this) - Scene::graphicals.begin();
	std::erase(Scene::graphicals, this);
	for (int i = ind; i < Scene::graphicals.size(); i++)
	{
		Scene::graphicals[i]->_indexId--;
		assert(Scene::graphicals[i]->_indexId == i);
	}

	delete _material;
}

Material* Graphical::material()
{
	if (_material) return _material;

	_material = new Material(*_sharedMaterial);
	BufferController::markBufferForUpdate(BufferType::Objects);
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

void Graphical::setScale(glm::vec3 scale, bool notify)
{
	Object::setScale(scale, notify);

	if (materialNoCopy()->emission() != Color::clear())
		BufferController::updateLights();
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

Mesh::Mesh(Model* model, glm::vec3 pos, glm::quat rot, glm::vec3 scale) : Graphical(pos, rot, scale), _model(model)
{
	init(model);
}
Mesh::Mesh(const Mesh& orig) : Graphical(orig), _model(orig._model)
{
	init(orig._model);
}
void Mesh::init(const Model* model)
{
	setModel(model);
}

Mesh::~Mesh()
{
	if (!initialized()) return;

	removeTriangles();
}
void Mesh::removeTriangles()
{
	if (_triangles.empty()) return;

	auto triStart = std::ranges::find(Scene::triangles, _triangles[0]);
	auto triEnd = std::ranges::find(Scene::triangles, _triangles.back());
	Scene::triangles.erase(triStart, triEnd + 1);

	for (auto& t : _triangles)
		delete t;
	_triangles.clear();

	BufferController::markBufferForUpdate(BufferType::Triangles);
}

void Mesh::setModel(const Model* model)
{
	removeTriangles();
	if (model == nullptr) return;

	_triangles.reserve(model->baseTriangles().size());
	for (auto& t : model->baseTriangles())
		_triangles.push_back(new Triangle(t, this));

	Scene::triangles.insert(Scene::triangles.end(), this->_triangles.begin(), this->_triangles.end());
	BufferController::markBufferForUpdate(BufferType::Triangles);
}

Square::Square() : _side(0)
{
	_model = getBaseModel();
}
Square::Square(glm::vec3 pos, float side, glm::quat rot, glm::vec3 scale) : Mesh(getBaseModel(), pos, rot, side * scale), _side(side) {}
Square::Square(const Square& orig) : Mesh(orig), _side(orig._side) {}

Model* Square::getBaseModel()
{
	static Model* _baseModel = nullptr;

	if (_baseModel != nullptr) return _baseModel;

	auto p1 = glm::vec3(-1 / 2.0f, 0, -1 / 2.0f);
	auto p2 = glm::vec3(-1 / 2.0f, 0, 1 / 2.0f);
	auto p3 = glm::vec3(1 / 2.0f, 0, 1 / 2.0f);
	auto p4 = glm::vec3(1 / 2.0f, 0, -1 / 2.0f);

	Vertex vertex1 {p1, {0, 0}};
	Vertex vertex2 {p2, {1, 0}};
	Vertex vertex3 {p3, {1, 1}};
	Vertex vertex4 {p4, {0, 1}};

	std::vector<BaseTriangle*> baseTris;
	baseTris.push_back(new BaseTriangle(vertex1, vertex2, vertex3));
	baseTris.push_back(new BaseTriangle(vertex1, vertex3, vertex4));

	return _baseModel = new Model(baseTris);
}

Cube::Cube(): _side(0)
{
	_model = getBaseModel();
}
Cube::Cube(glm::vec3 pos, float side, glm::quat rot, glm::vec3 scale) : Mesh(getBaseModel(), pos, rot, side * scale), _side(side) {}
Cube::Cube(const Cube& orig) : Mesh(orig), _side(orig._side) {}

Model* Cube::getBaseModel()
{
	static Model* _baseModel = nullptr;
	if (_baseModel != nullptr) return _baseModel;

	// Define cube vertices
	auto p1 = glm::vec3(-0.5f, -0.5f, -0.5f);
	auto p2 = glm::vec3(-0.5f, -0.5f, 0.5f);
	auto p3 = glm::vec3(0.5f, -0.5f, 0.5f);
	auto p4 = glm::vec3(0.5f, -0.5f, -0.5f);

	auto p5 = glm::vec3(-0.5f, 0.5f, -0.5f);
	auto p6 = glm::vec3(-0.5f, 0.5f, 0.5f);
	auto p7 = glm::vec3(0.5f, 0.5f, 0.5f);
	auto p8 = glm::vec3(0.5f, 0.5f, -0.5f);

	std::vector<BaseTriangle*> baseTris;

	baseTris.push_back(new BaseTriangle(p1, p3, p2));
	baseTris.push_back(new BaseTriangle(p1, p4, p3));
	baseTris[0]->setUVs({0, 0}, {1, 1}, {1, 0});
	baseTris[1]->setUVs({0, 0}, {0, 1}, {1, 1});

	baseTris.push_back(new BaseTriangle(p5, p6, p7));
	baseTris.push_back(new BaseTriangle(p5, p7, p8));
	baseTris[2]->setUVs({0, 0}, {1, 0}, {1, 1});
	baseTris[3]->setUVs({0, 0}, {1, 1}, {0, 1});

	baseTris.push_back(new BaseTriangle(p1, p2, p6));
	baseTris.push_back(new BaseTriangle(p1, p6, p5));
	baseTris[4]->setUVs({0, 0}, {1, 0}, {1, 1});
	baseTris[5]->setUVs({0, 0}, {1, 1}, {0, 1});

	baseTris.push_back(new BaseTriangle(p4, p7, p3));
	baseTris.push_back(new BaseTriangle(p4, p8, p7));
	baseTris[6]->setUVs({0, 0}, {1, 1}, {1, 0});
	baseTris[7]->setUVs({0, 0}, {0, 1}, {1, 1});

	baseTris.push_back(new BaseTriangle(p2, p3, p7));
	baseTris.push_back(new BaseTriangle(p2, p7, p6));
	baseTris[8]->setUVs({0, 0}, {1, 0}, {1, 1});
	baseTris[9]->setUVs({0, 0}, {1, 1}, {0, 1});

	baseTris.push_back(new BaseTriangle(p1, p8, p4));
	baseTris.push_back(new BaseTriangle(p1, p5, p8));
	baseTris[10]->setUVs({0, 0}, {1, 1}, {1, 0});
	baseTris[11]->setUVs({0, 0}, {0, 1}, {1, 1});

	return _baseModel = new Model(baseTris);
}

Sphere::Sphere(glm::vec3 pos, float radius, glm::vec3 scale) : Graphical(pos, {}, scale), _radius(radius) {}
Sphere::Sphere(const Sphere& orig) : Graphical(orig), _radius(orig._radius) {}

void Sphere::setRadius(float radius)
{
	_radius = radius;

	BufferController::markBufferForUpdate(BufferType::Objects);
}

Plane::Plane(glm::vec3 pos, glm::vec3 rot) : Graphical(pos, rot) {}
Plane::Plane(const Plane& orig) : Graphical(orig) {}
