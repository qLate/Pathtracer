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

class BaseTriangle
{
	std::vector<Vertex> _vertices;
	glm::vec3 _localNormal;

public:
	BaseTriangle(Vertex v1, Vertex v2, Vertex v3);

	std::vector<Vertex>& vertices() { return _vertices; }

	void setUVs(glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3);
};

class Triangle
{
	BaseTriangle* _baseTriangle;
	Mesh* _mesh;

	//std::vector<glm::vec3> _globalVertPositions {};
	//std::vector<glm::vec3> _globalVertNormals {};
	//glm::vec3 _globalNormal {};

public:
	void updateGeometry();

	Triangle(BaseTriangle* baseTri, Mesh* mesh);

	BaseTriangle* baseTriangle() const { return _baseTriangle; }
	std::vector<Vertex>& vertices() const { return _baseTriangle->vertices(); }
	Mesh* mesh() const { return _mesh; }

	//std::vector<glm::vec3>& globalVertPositions() { return _globalVertPositions; }
	//std::vector<glm::vec3>& globalVertNormals() { return _globalVertNormals; }

	AABB getBoundingBox() const;
	glm::vec3 getCenter() const;
};
