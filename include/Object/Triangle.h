#pragma once

#include <vector>
#include <glm/gtx/string_cast.hpp>

#include "BVH.h"

class Mesh;

struct Vertex
{
	glm::vec3 pos;
	glm::vec2 uvPos;
	glm::vec3 normal;

	Vertex(glm::vec3 pos = glm::vec3(), glm::vec2 uvPos = glm::vec2 {}, glm::vec3 normal = glm::vec3 {}) : pos(pos), uvPos(uvPos), normal(normal) {}
};

class Triangle
{
	Mesh* _mesh = nullptr;
	std::vector<Vertex> _vertices {};
	std::vector<glm::vec3> _globalVertPositions {};
	std::vector<glm::vec3> _globalVertNormals {};

	glm::vec3 _localNormal;
	glm::vec3 _globalNormal {};

public:
	void updateGeometry();

	Triangle(Vertex v1, Vertex v2, Vertex v3, Mesh* mesh = nullptr);
	void attachTo(Mesh* mesh);

	Mesh* mesh() const { return _mesh; }
	std::vector<Vertex>& vertices() { return _vertices; }
	std::vector<glm::vec3>& globalVertPositions() { return _globalVertPositions; }
	std::vector<glm::vec3>& globalVertNormals() { return _globalVertNormals; }

	AABB getBoundingBox() const;
	glm::vec3 getCenter() const;
};
