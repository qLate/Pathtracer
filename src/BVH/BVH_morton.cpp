#include <algorithm>

#include "BVH.h"
#include "MortonCodes.h"
#include "Triangle.h"

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

	nodes.push_back(nullptr);
	nodes[0] = new BVHNodeMorton(nodes, sortedTriangles, 0, (int)triangles.size() - 1);
}

BVHNodeMorton::BVHNodeMorton(std::vector<BVHNode*>& nodes, std::vector<std::pair<uint32_t, Triangle*>>& triangles, int start, int end, int nextRightNode)
{
	missNext = nextRightNode;
	if (end - start < BVHBuilder::MAX_TRIANGLES_PER_BOX)
	{
		setLeaf([&triangles](int i) { return triangles[i].second; }, start, end);
		return;
	}

	int splitIdx = getSplitIndex(triangles, start, end);

	leftInd = (int)BVHBuilder::nodes.size();
	rightInd = (int)BVHBuilder::nodes.size() + 1;
	hitNext = leftInd;

	nodes.push_back(nullptr);
	nodes.push_back(nullptr);
	nodes[leftInd] = new BVHNodeMorton(nodes, triangles, start, splitIdx, rightInd);
	nodes[rightInd] = new BVHNodeMorton(nodes, triangles, splitIdx + 1, end, nextRightNode);

	box = AABB::getUnitedBox(nodes[leftInd]->box, nodes[rightInd]->box);
}
int BVHNodeMorton::getSplitIndex(const std::vector<std::pair<uint32_t, Triangle*>>& triangles, int start, int end)
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
