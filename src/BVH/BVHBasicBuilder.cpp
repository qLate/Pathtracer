#include "BVHBasicBuilder.h"

#include <algorithm>

#include "Triangle.h"
#include "glm/common.hpp"

static auto& nodes = BVH::nodes;

void BVHBasicBuilder::buildTreeBasic(std::vector<Triangle*>& triangles)
{
	nodes.clear();
	nodes.push_back(nullptr);

	nodes[0] = new BVHNodeBasic(nodes, triangles, 0, (int)triangles.size() - 1);
}

BVHBasicBuilder::BVHNodeBasic::BVHNodeBasic(std::vector<BVHNode*>& nodes, std::vector<Triangle*>& triangles, int start, int end, int nextRightNode)
{
	missNext = nextRightNode;
	if (end - start < BVH::MAX_TRIANGLES_PER_BOX)
	{
		setLeaf([&triangles](int i) { return triangles[i]; }, start, end);
		return;
	}

	int splitIdx = getSplitIndex(triangles, start, end);

	leftInd = (int)BVH::nodes.size();
	rightInd = (int)BVH::nodes.size() + 1;
	hitNext = leftInd;

	nodes.push_back(nullptr);
	nodes.push_back(nullptr);
	nodes[leftInd] = new BVHNodeBasic(nodes, triangles, start, splitIdx, rightInd);
	nodes[rightInd] = new BVHNodeBasic(nodes, triangles, splitIdx + 1, end, nextRightNode);

	box = AABB::getUnitedBox(nodes[leftInd]->box, nodes[rightInd]->box);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
int BVHBasicBuilder::BVHNodeBasic::getSplitIndex(std::vector<Triangle*>& triangles, int start, int end)
{
	glm::vec3 min {FLT_MAX}, max {-FLT_MAX};
	for (int i = start; i <= end; i++)
	{
		auto pos = triangles[i]->getCenter();
		min = glm::min(min, pos);
		max = glm::max(max, pos);
	}

	auto range = max - min;
	int axis = 0;
	if (range.y > range.x) axis = 1;
	if (range.z > range[axis]) axis = 2;
	float splitPos = min[axis] + range[axis] * 0.5f;

	std::sort(triangles.begin() + start, triangles.begin() + end + 1, [axis](auto a, auto b) { return a->getCenter()[axis] < b->getCenter()[axis]; });

	auto split = start;
	while (triangles[split]->getCenter()[axis] < splitPos && split < end - 1) split++;
	return split;
}
