#include "BVH.h"

#include <algorithm>

#include "Scene.h"
#include "Triangle.h"

void BVHBuilder::initializeBVH()
{
	buildTree(Scene::triangles);
}

void BVHBuilder::buildTree(const std::vector<Triangle*>& objects)
{
	nodes.clear();

	nodes.push_back(nullptr);
	nodes[0] = std::make_shared<BVHNode>(nodes, Scene::triangles, 0, (int)Scene::triangles.size() - 1, MAX_TRIANGLES_PER_BOX);
}

AABB AABB::getUnitedBox(const AABB& box1, const AABB& box2)
{
	auto x_min = std::min(box1.min.x, box2.min.x);
	auto x_max = std::max(box1.max.x, box2.max.x);

	auto y_min = std::min(box1.min.y, box2.min.y);
	auto y_max = std::max(box1.max.y, box2.max.y);

	auto z_min = std::min(box1.min.z, box2.min.z);
	auto z_max = std::max(box1.max.z, box2.max.z);
	return {{x_min, y_min, z_min}, {x_max, y_max, z_max}};
}

BVHNode::BVHNode(std::vector<std::shared_ptr<BVHNode>>& nodes, std::vector<Triangle*>& triangles, int start, int end, int maxTrianglesPerBox, int nextRightNode)
{
	missNext = nextRightNode;
	if (end - start < maxTrianglesPerBox)
	{
		isLeaf = true;
		hitNext = missNext;
		leafTrianglesStart = start;
		leafTriangleCount = end - start + 1;
		box = triangles[start]->getBoundingBox();
		for (int i = start + 1; i <= end; ++i)
			box = AABB::getUnitedBox(box, triangles[i]->getBoundingBox());
		return;
	}

	int splitIdx = getSplitIndex(triangles, start, end);

	leftInd = (int)BVHBuilder::nodes.size();
	rightInd = (int)BVHBuilder::nodes.size() + 1;
	hitNext = leftInd;

	nodes.push_back(nullptr);
	nodes.push_back(nullptr);
	nodes[leftInd] = std::make_shared<BVHNode>(nodes, triangles, start, splitIdx, maxTrianglesPerBox, rightInd);
	nodes[rightInd] = std::make_shared<BVHNode>(nodes, triangles, splitIdx + 1, end, maxTrianglesPerBox, nextRightNode);

	box = AABB::getUnitedBox(nodes[leftInd]->box, nodes[rightInd]->box);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
int BVHNode::getSplitIndex(std::vector<Triangle*>& triangles, int start, int end)
{
	glm::vec3 min {FLT_MAX}, max {-FLT_MAX};
	for (int i = start; i <= end; i++)
	{
		auto pos = triangles[i]->getCenter();
		min.x = std::min(min.x, pos.x);
		min.y = std::min(min.y, pos.y);
		min.z = std::min(min.z, pos.z);

		max.x = std::max(max.x, pos.x);
		max.y = std::max(max.y, pos.y);
		max.z = std::max(max.z, pos.z);
	}

	auto range = max - min;
	int axis = 0;
	if (range.y > range.x) axis = 1;
	if (range.z > range[axis]) axis = 2;
	float splitPos = min[axis] + range[axis] * 0.5f;

	std::sort(triangles.begin() + start, triangles.begin() + end + 1,
	          [axis](auto a, auto b) { return a->getCenter()[axis] < b->getCenter()[axis]; });
	auto splitIdx = start;
	while (triangles[splitIdx]->getCenter()[axis] < splitPos && splitIdx < end - 1) splitIdx++;
	return splitIdx;
}
