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

	Vertex(glm::vec3 pos, glm::vec2 uvPos = glm::vec2 {}, glm::vec3 normal = glm::vec3 {}) : pos(pos), uvPos(uvPos), normal(normal) {}
};

class Triangle
{
	glm::vec3 localNormal;

public:
	Mesh* mesh = nullptr;

	std::vector<Vertex> vertices;
	std::vector<glm::vec3> globalVertexPositions;
	std::vector<glm::vec3> globalVertexNormals;
	glm::vec3 globalNormal = {};

	void updateGeometry();

	// Precalculated
	glm::vec3 row1 {};
	glm::vec3 row2 {};
	glm::vec3 row3 {};
	float row1Val {};
	float row2Val {};
	float row3Val {};

	Triangle(Mesh* mesh, Vertex v1, Vertex v2, Vertex v3);
	void attachTo(Mesh* mesh);
	void recalculateCoefficients();

	AABB getBoundingBox() const;
	glm::vec3 getCenter() const;
};
