#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <glm/vec3.hpp>

#include "Utils.h"

class BVHBuilder;
class Triangle;
class BVHNode;

struct Link
{
	int hit;
	int miss;
};

class BVH
{
public:
	static constexpr int MAX_TRIANGLES_PER_BOX = 1;

	inline static std::vector<BVHNode*> nodes;
	inline static std::vector<int> originalTriIndices;

	inline static UPtr<BVHBuilder> builder;

	static void init();

	static void buildBVH();
	static void rebuildBVH();
};

class AABB
{
public:
	glm::vec3 min_, max_;

	AABB() : min_(), max_() {}
	AABB(glm::vec3 min, glm::vec3 max) : min_(min), max_(max) {}

	static AABB getUnitedBox(const AABB& box1, const AABB& box2);
	glm::vec3 getCenter() const;

	bool isZero() const { return min_ == glm::vec3(0) && max_ == glm::vec3(0); }
};

class BVHNode
{
public:
	int parent = -1;
	int left = -1, right = -1;
	bool isLeaf = false;
	int leafTrianglesStart = -1, leafTriangleCount = -1;
	int hitNext = -1, missNext = -1;

	AABB box;

	void setLeaf(const std::function<Triangle*(int)>& triangleGetter, int start, int end);
};

class BVHBuilder
{
public:
	virtual ~BVHBuilder() = default;

	virtual void build() = 0;
	virtual void rebuild() { build(); }
};
