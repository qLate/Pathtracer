#include "Triangle.h"

#include "BVH.h"
#include "Graphical.h"

Triangle::Triangle(Vertex v1, Vertex v2, Vertex v3, Mesh* mesh) : _localNormal(normalize(cross(v2.pos - v1.pos, v3.pos - v2.pos))), _vertices({v1, v2, v3})
{
	for (auto& v : _vertices)
	{
		if (v.normal == glm::vec3(0, 0, 0))
			v.normal = _localNormal;
	}

	if (mesh != nullptr)
		attachTo(mesh);
}
void Triangle::attachTo(Mesh* mesh)
{
	this->_mesh = mesh;

	updateGeometry();
}

AABB Triangle::getBoundingBox() const
{
	float x_min = FLT_MAX, x_max = -FLT_MAX;
	float y_min = FLT_MAX, y_max = -FLT_MAX;
	float z_min = FLT_MAX, z_max = -FLT_MAX;

	for (const auto& pos : _globalVertPositions)
	{
		x_min = std::min(x_min, pos.x);
		x_max = std::max(x_max, pos.x);
		y_min = std::min(y_min, pos.y);
		y_max = std::max(y_max, pos.y);
		z_min = std::min(z_min, pos.z);
		z_max = std::max(z_max, pos.z);
	}
	return {{x_min - 0.0001f, y_min - 0.0001f, z_min - 0.0001f}, {x_max + 0.0001f, y_max + 0.0001f, z_max + 0.0001f}};
}

glm::vec3 Triangle::getCenter() const
{
	return (_globalVertPositions[0] + _globalVertPositions[1] + _globalVertPositions[2]) * 0.33333f;
}

void Triangle::updateGeometry()
{
	return;
	_globalVertPositions = {
		_mesh->localToGlobalPos(_vertices[0].pos),
		_mesh->localToGlobalPos(_vertices[1].pos),
		_mesh->localToGlobalPos(_vertices[2].pos)
	};

	_globalNormal = _mesh->rot() * _localNormal;

	_globalVertNormals = {
		_mesh->rot() * _vertices[0].normal,
		_mesh->rot() * _vertices[1].normal,
		_mesh->rot() * _vertices[2].normal
	};
}
