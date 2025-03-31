#pragma once

#include <vector>

#include "JsonUtility.h"
#include "Triangle.h"

class Triangle;

class Model
{
	std::string _path;
	std::vector<BaseTriangle*> _baseTriangles;

	Model(const std::filesystem::path& path);
	void parse(const std::filesystem::path& path);
	void parseRapidobj(const std::filesystem::path& path);
	void parseSelfWritten(const std::filesystem::path& path);
	Model(const Model& other);

public:
	Model(const std::vector<BaseTriangle*>& baseTriangles);
	Model(const std::vector<glm::vec3>& points);
	Model() = default;

	std::vector<BaseTriangle*> baseTriangles() const { return _baseTriangles; }

	constexpr static auto properties();

	friend class Assets;
	friend class JsonUtility;
};

constexpr auto Model::properties()
{
	return std::make_tuple(
		JsonUtility::property(&Model::_path, "path")
	);
}
