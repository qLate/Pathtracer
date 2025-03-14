#include "BVHMortonBuilder.h"

#include <algorithm>
#include <bit>
#include <stack>

#include "Debug.h"
#include "MortonCodes.h"
#include "Triangle.h"
#include "Utils.h"

static auto& nodes = BVH::nodes;
static auto& originalTriIndices = BVH::originalTriIndices;

void BVHMortonBuilder::build(const std::vector<Triangle*>& triangles)
{
	nodes.clear();

	auto sortedTriangleIndices = getSortedTrianglesByMorton(triangles);
	recordOriginalTriIndices(triangles, sortedTriangleIndices);

	buildStacked(triangles, sortedTriangleIndices);
	calculateBoxes();
}

std::vector<std::pair<uint32_t, int>> BVHMortonBuilder::getSortedTrianglesByMorton(const std::vector<Triangle*>& triangles)
{
	auto centers = std::vector<glm::vec3>(triangles.size());
	for (int i = 0; i < triangles.size(); i++)
		centers[i] = triangles[i]->getCenter();

	auto mortonCodes = MortonCodes::generateMortonCodes(centers);

	auto sortedTriangleIndices = std::vector<std::pair<uint32_t, int>>(triangles.size());
	for (int i = 0; i < triangles.size(); i++)
		sortedTriangleIndices[i] = {mortonCodes[i], i};

	std::ranges::sort(sortedTriangleIndices, [](auto a, auto b) { return a.first < b.first; });
	return sortedTriangleIndices;
}

void BVHMortonBuilder::recordOriginalTriIndices(const std::vector<Triangle*>& triangles, const std::vector<std::pair<uint32_t, int>>& sortedTriangleIndices)
{
	originalTriIndices.resize(triangles.size());
	for (int i = 0; i < triangles.size(); i++)
		originalTriIndices[i] = sortedTriangleIndices[i].second;
}

void BVHMortonBuilder::buildStacked(const std::vector<Triangle*>& triangles, std::vector<std::pair<uint32_t, int>> sortedTriangleIndices)
{
	struct StackEntry
	{
		int nodeIdx, start, end, nextRightNode;
	};

	nodes.reserve(2 * triangles.size());
	nodes.push_back(new BVHNode());
	std::stack<StackEntry> stack;

	stack.push({0, 0, (int)triangles.size() - 1, -1});

	while (!stack.empty())
	{
		auto [nodeIdx, start, end, nextRightNode] = stack.top();
		stack.pop();

		BVHNode* node = nodes[nodeIdx];
		node->missNext = nextRightNode;

		if (end - start < BVH::MAX_TRIANGLES_PER_BOX)
		{
			node->setLeaf([&triangles, &sortedTriangleIndices](int i) { return triangles[sortedTriangleIndices[i].second]; }, start, end);
			continue;
		}

		int leftInd = nodes.size();
		int rightInd = nodes.size() + 1;
		node->leftInd = leftInd;
		node->rightInd = rightInd;
		node->hitNext = leftInd;

		nodes.push_back(new BVHNode());
		nodes.push_back(new BVHNode());

		int splitIdx = getSplitIndex(sortedTriangleIndices, start, end);
		stack.push({rightInd, splitIdx + 1, end, nextRightNode});
		stack.push({leftInd, start, splitIdx, rightInd});
	}
}

void BVHMortonBuilder::calculateBoxes()
{
	for (int i = nodes.size() - 1; i >= 0; i--)
	{
		auto node = nodes[i];
		if (node->isLeaf) continue;

		auto left = nodes[node->leftInd];
		auto right = nodes[node->rightInd];
		node->box = AABB::getUnitedBox(left->box, right->box);
	}
}

int BVHMortonBuilder::getSplitIndex(const std::vector<std::pair<uint32_t, int>>& sortedIndices, int start, int end)
{
	uint32_t firstCode = sortedIndices[start].first;
	uint32_t lastCode = sortedIndices[end].first;
	if (firstCode == lastCode) return (start + end) / 2;

	int commonPref = MortonCodes::commonPrefixLength(firstCode, lastCode);

	int split = start;
	int step = end - start;
	do
	{
		step = (step + 1) >> 1;
		int newSplit = split + step;

		if (newSplit < end)
		{
			unsigned int splitCode = sortedIndices[newSplit].first;
			int splitPrefix = std::countl_zero(firstCode ^ splitCode) - 1;
			if (splitPrefix > commonPref)
				split = newSplit;
		}
	}
	while (step > 1);
	return split;
}
