#include "Graphical.h"

#include <fstream>

#include "Camera.h"
#include "Scene.h"
#include "Triangle.h"
#include "Debug.h"

#include "rapidobj.hpp"
#include "Utils.h"


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
	triangles.push_back(new Triangle(vertex1, vertex2, vertex3, this));
	triangles.push_back(new Triangle(vertex1, vertex3, vertex4, this));
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

Sphere::Sphere(glm::vec3 pos, float radius) : Graphical(pos, {}), radius(radius) {}

Plane::Plane(glm::vec3 pos, glm::vec3 normal) : Graphical({}, pos), normal {normalize(normal)} {}


Model::Model(const std::filesystem::path& path)
{
	parse(path);
}
void Model::parse(const std::filesystem::path& path)
{
	parseRapidobj(path);
}

void Model::parseRapidobj(const std::filesystem::path& path)
{
	using namespace rapidobj;
	Result result = ParseFile(path, MaterialLibrary::Default(Load::Optional));
	if (result.error) Debug::logError("Error loading OBJ file: ", result.error.code.message());

	bool success = Triangulate(result);
	if (!success) Debug::logError("Triangulation failed!");

	std::vector<Vertex> vertices(3);
	for (const auto& shape : result.shapes)
	{
		const auto& mesh = shape.mesh;
		const auto& attributes = result.attributes;

		size_t indexOffset = 0;
		for (size_t f = 0; f < mesh.num_face_vertices.size(); f++)
		{
			if (mesh.num_face_vertices[f] != 3) continue; // Skip non-triangles (should be triangulated)

			for (size_t v = 0; v < 3; v++)
			{
				const auto& [posIdx, uvIdx, normalIdx] = mesh.indices[indexOffset + v];

				vertices[v] = Vertex();

				vertices[v].pos.x = attributes.positions[posIdx * 3 + 0];
				vertices[v].pos.y = attributes.positions[posIdx * 3 + 1];
				vertices[v].pos.z = attributes.positions[posIdx * 3 + 2];

				if (normalIdx >= 0 && normalIdx < attributes.normals.size() / 3)
				{
					vertices[v].normal.x = attributes.normals[normalIdx * 3 + 0];
					vertices[v].normal.y = attributes.normals[normalIdx * 3 + 1];
					vertices[v].normal.z = attributes.normals[normalIdx * 3 + 2];
				}

				if (uvIdx >= 0 && uvIdx < attributes.texcoords.size() / 2)
				{
					vertices[v].uvPos.x = Utils::mod(attributes.texcoords[uvIdx * 2 + 0], 1.0f);
					vertices[v].uvPos.y = Utils::mod(attributes.texcoords[uvIdx * 2 + 1], 1.0f);
				}
			}

			triangles.push_back(new Triangle(vertices[0], vertices[1], vertices[2]));
			indexOffset += 3;
		}
	}
}

void Model::parseSelfWritten(const std::filesystem::path& path)
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
				uv.push_back(Utils::mod(std::stof(token), 1.0f));

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
				int ind1 = str.find('/');
				substr = str.substr(0, ind1);
				num1 = std::stoi(substr);

				if (ind1 != std::string::npos)
				{
					int ind2 = str.find('/', ind1 + 1);
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
				auto v1 = Vertex();
				auto v2 = Vertex();
				auto v3 = Vertex();

				v1.pos = vertexPositions[posIndexes[0] % vertexPositions.size()];
				v2.pos = vertexPositions[posIndexes[i - 1] % vertexPositions.size()];
				v3.pos = vertexPositions[posIndexes[i] % vertexPositions.size()];

				if (uvIndexes[0] != -1)
				{
					v1.uvPos = vertexUVs[uvIndexes[0]];
					v2.uvPos = vertexUVs[uvIndexes[i - 1]];
					v3.uvPos = vertexUVs[uvIndexes[i]];
				}

				if (normalIndexes[0] != -1)
				{
					v1.normal = vertexNormals[normalIndexes[0]];
					v2.normal = vertexNormals[normalIndexes[i - 1]];
					v3.normal = vertexNormals[normalIndexes[i]];
				}

				triangles.emplace_back(new Triangle(v1, v2, v3));
			}
		}
	}
}
