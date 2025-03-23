#pragma once

#include <filesystem>
#include <vector>

#include "Triangle.h"

class Triangle;

class Model
{
	std::vector<BaseTriangle*> _baseTriangles;

	explicit Model(const std::filesystem::path& path);
	void parse(const std::filesystem::path& path);
	void parseRapidobj(const std::filesystem::path& path);
	void parseSelfWritten(const std::filesystem::path& path);

public:
	Model(const std::vector<BaseTriangle*>& baseTriangles);

	std::vector<BaseTriangle*> baseTriangles() const { return _baseTriangles; }

	friend class Assets;
};
