#include "Model.h"

#include <fstream>

#include "BufferController.h"
#include "Debug.h"
#include "MyMath.h"
#include "rapidobj.hpp"
#include "Scene.h"

Model::Model(const std::filesystem::path& path) : _path(path.string())
{
	parse(path);

	init();
}
Model::Model(const Model& other) : _path(other._path)
{
	parse(std::filesystem::path(_path));

	init();
}

void Model::init()
{
	Scene::models.push_back(this);

	_triStartIndex = Scene::baseTriangles.size();
	Scene::baseTriangles.insert(Scene::baseTriangles.end(), this->_baseTriangles.begin(), this->_baseTriangles.end());
	BufferController::markBufferForUpdate(BufferType::Triangles);
}

void Model::parse(const std::filesystem::path& path)
{
	parseRapidobj(path);
}

Model::Model(const std::vector<BaseTriangle*>& baseTriangles)
{
	_baseTriangles = baseTriangles;

	init();
}

Model::~Model()
{
	std::erase(Scene::models, this);

	auto fromInd = std::ranges::find(Scene::baseTriangles, _baseTriangles[0]);
	auto toInd = std::ranges::find(Scene::baseTriangles, _baseTriangles.back());
	if (toInd - fromInd >= _baseTriangles.size()) throw std::exception("Base triangles weren't contiguous in memory.");
	for (auto it = fromInd; it!= toInd; ++it)
		*it = nullptr;

	for (auto& triangle : _baseTriangles)
		delete triangle;
}

void Model::setBvhRootNode(int bvhRootNode)
{
	_bvhRootNode = bvhRootNode;
}

void Model::parseRapidobj(const std::filesystem::path& path)
{
	using namespace rapidobj;
	auto mtlLib = MaterialLibrary::Default(Load::Optional);
	Result result = ParseFile(path, mtlLib);
	if (result.error) Debug::logError("Error loading OBJ file: ", result.error.code.message());

	bool success = Triangulate(result);
	if (!success) Debug::logError("Triangulation failed!");

	std::atomic f = 0;
	for (const auto& shape : result.shapes)
	{
		const auto& mesh = shape.mesh;
		const auto& attributes = result.attributes;

		_baseTriangles.resize(_baseTriangles.size() + mesh.num_face_vertices.size());
		#ifndef NDEBUG
		std::vector<Vertex> vertices(3);
		#else
		#pragma omp parallel for
		#endif
		for (int j = 0; j < mesh.num_face_vertices.size(); j++)
		{
			if (mesh.num_face_vertices[j] != 3) throw std::runtime_error("Non triangle found after triangulation.");
			#ifdef NDEBUG
			std::vector<Vertex> vertices(3);
			#endif
			for (int v = 0; v < 3; v++)
			{
				const auto& [posIdx, uvIdx, normalIdx] = mesh.indices[j * 3 + v];
				#ifndef NDEBUG
				vertices[v] = Vertex();
				#endif
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
					vertices[v].uvPos.x = Math::mod(attributes.texcoords[uvIdx * 2 + 0], 1.0f);
					vertices[v].uvPos.y = Math::mod(attributes.texcoords[uvIdx * 2 + 1], 1.0f);
				}
			}

			_baseTriangles[f++] = new BaseTriangle(vertices[0], vertices[1], vertices[2]);
		}
	}

	for (int i = 0; i < _baseTriangles.size(); i++)
	{
		if (_baseTriangles[i] == nullptr)
			Debug::log("Triangle ", i);
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
				uv.push_back(Math::mod(std::stof(token), 1.0f));

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

				_baseTriangles.emplace_back(new BaseTriangle(v1, v2, v3));
			}
		}
	}
}
