#include "BVH.h"

#include "BVHMortonBuilder.h"
#include "Scene.h"
#include "Triangle.h"
#include "Utils.h"

void BVH::init()
{
	builder = make_unique<BVHMortonBuilder>();
}

void BVH::buildBVH()
{
	builder->build(Scene::triangles);
}
void BVH::rebuildBVH()
{
 	builder->rebuild(Scene::triangles);
}

AABB AABB::getUnitedBox(const AABB& box1, const AABB& box2)
{
	return {min(box1.min_, box2.min_), max(box1.max_, box2.max_)};
}
glm::vec3 AABB::getCenter() const
{
	return (min_ + max_) * 0.5f;
}

void BVHNode::setLeaf(const std::function<Triangle*(int)>& triangleGetter, int start, int end)
{
	isLeaf = true;
	hitNext = missNext;
	leafTrianglesStart = start;
	leafTriangleCount = end - start + 1;
	box = triangleGetter(start)->getBoundingBox();
	for (int i = start + 1; i <= end; ++i)
		box = AABB::getUnitedBox(box, triangleGetter(i)->getBoundingBox());
}
