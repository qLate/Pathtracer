#pragma once

#include <filesystem>
#include <vector>

#include "Triangle.h"

class Triangle;

class Model
{
	void parse(const std::filesystem::path& path);

public:
	std::vector<Triangle*> triangles;
	std::vector<Vertex> vertices;

	explicit Model(const std::filesystem::path& path);
	void parseRapidobj(const std::filesystem::path& path);
	void parseSelfWritten(const std::filesystem::path& path);
};
