#pragma once

#include <vector>

#include "JsonUtility.h"
#include "Triangle.h"

class Triangle;

class Model
{
	std::string _path;
	std::vector<BaseTriangle*> _baseTriangles;

	int _triStartIndex = -1;
	int _bvhRootNode = -1;

	Model(const std::filesystem::path& path);
	void parse(const std::filesystem::path& path);
	void parseRapidobj(const std::filesystem::path& path);
	void parseSelfWritten(const std::filesystem::path& path);
	Model(const Model& other);

	void init();

public:
	Model(const std::vector<BaseTriangle*>& baseTriangles);
	Model() = default;

	~Model();

	std::vector<BaseTriangle*> baseTriangles() const { return _baseTriangles; }
	int bvhRootNode() const { return _bvhRootNode; }

	void setBvhRootNode(int bvhRootNode);

	int triStartIndex() const { return _triStartIndex;  }

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
