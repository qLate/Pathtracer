#include "BVH.h"

#include <algorithm>

#include "Scene.h"
#include "Triangle.h"
#include "Utils.h"
#include "MortonCodes.h"

void BVHBuilder::initBVH()
{
	buildTreeMorton(Scene::triangles);
}

void BVHBuilder::buildTreeBasic(std::vector<Triangle*>& triangles)
{
	nodes.clear();

	auto root = new BVHNodeBasic(nodes, triangles, 0, (int)triangles.size() - 1, MAX_TRIANGLES_PER_BOX);
	nodes.push_back(root);
}
void BVHBuilder::buildTreeMorton(std::vector<Triangle*>& triangles)
{
	nodes.clear();

	auto centers = std::vector<glm::vec3>(triangles.size());
	for (size_t i = 0; i < triangles.size(); i++)
		centers[i] = triangles[i]->getCenter();

	auto mortonCodes = MortonCodes::generateMortonCodes(centers);

	std::vector<std::pair<uint32_t, Triangle*>> sortedTriangles(triangles.size());
	for (size_t i = 0; i < triangles.size(); i++)
		sortedTriangles[i] = {mortonCodes[i], triangles[i]};

	std::ranges::sort(sortedTriangles, [](auto a, auto b) { return a.first < b.first; });

	for (size_t i = 0; i < triangles.size(); i++)
		triangles[i] = sortedTriangles[i].second;

	auto root = new BVHNodeMorton(nodes, sortedTriangles, 0, (int)triangles.size() - 1, MAX_TRIANGLES_PER_BOX);
	nodes.push_back(root);
}

AABB AABB::getUnitedBox(const AABB& box1, const AABB& box2)
{
	return {min(box1.min_, box2.min_), max(box1.max_, box2.max_)};
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

BVHNodeBasic::BVHNodeBasic(std::vector<BVHNode*>& nodes, std::vector<Triangle*>& triangles, int start, int end, int maxTrianglesPerBox, int nextRightNode)
{
	missNext = nextRightNode;
	if (end - start < maxTrianglesPerBox)
	{
		setLeaf([&triangles](int i) { return triangles[i]; }, start, end);
		return;
	}

	int splitIdx = getSplitIndex(triangles, start, end);

	leftInd = (int)BVHBuilder::nodes.size();
	rightInd = (int)BVHBuilder::nodes.size() + 1;
	hitNext = leftInd;

	nodes.push_back(nullptr);
	nodes.push_back(nullptr);
	nodes[leftInd] = new BVHNodeBasic(nodes, triangles, start, splitIdx, maxTrianglesPerBox, rightInd);
	nodes[rightInd] = new BVHNodeBasic(nodes, triangles, splitIdx + 1, end, maxTrianglesPerBox, nextRightNode);

	box = AABB::getUnitedBox(nodes[leftInd]->box, nodes[rightInd]->box);
}
int BVHNodeBasic::getSplitIndex(std::vector<Triangle*>& triangles, int start, int end)
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

BVHNodeMorton::BVHNodeMorton(std::vector<BVHNode*>& nodes, std::vector<std::pair<uint32_t, Triangle*>>& triangles, int start, int end, int maxTrianglesPerBox,
                             int nextRightNode, int depth)
{
	missNext = nextRightNode;
	if (end - start < maxTrianglesPerBox)
	{
		setLeaf([&triangles](int i) { return triangles[i].second; }, start, end);
		return;
	}

	int splitIdx = getSplitIndex(triangles, start, end, depth);

	leftInd = (int)BVHBuilder::nodes.size();
	rightInd = (int)BVHBuilder::nodes.size() + 1;
	hitNext = leftInd;

	nodes.push_back(nullptr);
	nodes.push_back(nullptr);
	nodes[leftInd] = new BVHNodeMorton(nodes, triangles, start, splitIdx, maxTrianglesPerBox, rightInd, depth);
	nodes[rightInd] = new BVHNodeMorton(nodes, triangles, splitIdx + 1, end, maxTrianglesPerBox, nextRightNode, depth);

	box = AABB::getUnitedBox(nodes[leftInd]->box, nodes[rightInd]->box);
}
int BVHNodeMorton::getSplitIndex(const std::vector<std::pair<uint32_t, Triangle*>>& triangles, int start, int end, int depth)
{
	uint32_t firstCode = triangles[start].first;
	uint32_t lastCode = triangles[end].first;

	if (firstCode == lastCode) return (start + end) / 2;

	int pref = MortonCodes::commonPrefixLength(firstCode, lastCode);
	int splitBit = 30 - pref;

	int split = start;
	for (int i = start + 1; i <= end; ++i)
	{
		if (triangles[i].first >> splitBit != firstCode >> splitBit)
		{
			split = i;
			break;
		}
	}
	return split - 1;
}
