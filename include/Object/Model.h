#pragma once

#include <filesystem>
#include <vector>

#include "Triangle.h"

class Triangle;

class Model
{
	std::vector<Triangle*> _triangles;

	void parse(const std::filesystem::path& path);

public:
	explicit Model(const std::filesystem::path& path);
	void parseRapidobj(const std::filesystem::path& path);
	void parseSelfWritten(const std::filesystem::path& path);

	std::vector<Triangle*> triangles() const { return _triangles; }
};
