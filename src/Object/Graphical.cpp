#include "Graphical.h"

#include <fstream>
#include <utility>

#include "Camera.h"
#include "Scene.h"
#include "Triangle.h"

Graphical::Graphical(glm::vec3 pos, glm::quat rot) : Object(pos, rot)
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
	if (_material)
	{
		delete _material;
		_material = nullptr;
	}

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

	Vertex vertex1 {p1, {0, 0}};
	Vertex vertex2 {p2, {1, 0}};
	Vertex vertex3 {p3, {1, 1}};
	Vertex vertex4 {p4, {0, 1}};

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

Sphere::Sphere(glm::vec3 pos, float radius) : Graphical(pos, {}), radius(radius) { }

Plane::Plane(glm::vec3 pos, glm::vec3 normal) : Graphical({}, pos), normal {normalize(normal)} { }


Model::Model(const std::filesystem::path& path)
{
	parse(path);
}

constexpr char DELIM = '/';
void Model::parse(const std::filesystem::path& path)
{
	std::ifstream file(path);
	std::string line;

	std::vector<glm::vec3> vertexPositions;
	std::vector<glm::vec2> vertexUVs;
	std::vector<glm::vec3> vertexNormals;
	while (std::getline(file, line))
	{
		std::stringstream ss(line);
		std::string token;
		ss >> token;
		if (token == "v")
		{
			std::vector<float> pos;
			while (ss >> token)
				pos.push_back(std::stof(token));

			vertexPositions.emplace_back(pos[0], pos[1], pos[2]);
		}
		else if (token == "vt")
		{
			std::vector<float> uv;
			while (ss >> token)
				uv.push_back(std::stof(token));

			vertexUVs.emplace_back(uv[0], uv[1]);
		}
		else if (token == "vn")
		{
			std::vector<float> normal;
			while (ss >> token)
				normal.push_back(std::stof(token));

			vertexNormals.push_back(normalize(glm::vec3(normal[0], normal[1], normal[2])));
		}
		else if (token == "f")
		{
			std::vector<int> posIndexes;
			std::vector<int> uvIndexes;
			std::vector<int> normalIndexes;

			int num1 = 0, num2 = 0, num3 = 0;
			std::string str, substr;
			while (ss >> str)
			{
				int ind1 = str.find(DELIM);
				substr = str.substr(0, ind1);
				num1 = std::stoi(substr);

				if (ind1 != std::string::npos)
				{
					int ind2 = str.find(DELIM, ind1 + 1);
					substr = str.substr(ind1 + 1, ind2);
					num2 = std::stoi(substr);

					if (ind2 != std::string::npos)
					{
						substr = str.substr(ind2 + 1);
						num3 = std::stoi(substr);
					}
				}

				posIndexes.push_back(num1 - 1);
				uvIndexes.push_back(num2 - 1);
				normalIndexes.push_back(num3 - 1);
			}

			for (int i = 2; i < posIndexes.size(); i++)
			{
				auto v1 = Vertex(vertexPositions[posIndexes[0] % vertexPositions.size()], uvIndexes[0] != -1 ? vertexUVs[uvIndexes[0]] : glm::vec2(),
					normalIndexes[0] != -1 ? vertexNormals[normalIndexes[0]] : glm::vec3(0, 0, 0));
				auto v2 = Vertex(vertexPositions[posIndexes[i - 1] % vertexPositions.size()], uvIndexes[i - 1] != -1 ? vertexUVs[uvIndexes[i - 1]] : glm::vec3(),
					normalIndexes[i - 1] != -1 ? vertexNormals[normalIndexes[i - 1]] : glm::vec3(0, 0, 0));
				auto v3 = Vertex(vertexPositions[posIndexes[i] % vertexPositions.size()], uvIndexes[i] != -1 ? vertexUVs[uvIndexes[i]] : glm::vec2(),
					normalIndexes[i - 1] != -1 ? vertexNormals[normalIndexes[i]] : glm::vec3(0, 0, 0));

				triangles.emplace_back(new Triangle(nullptr, v1, v2, v3));
			}
		}
	}
}
