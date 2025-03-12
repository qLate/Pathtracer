#include <algorithm>

#include "BVH.h"
#include "Triangle.h"
#include "glm/common.hpp"



BVHNodeSAH::BVHNodeSAH(std::vector<BVHNode*>& nodes, std::vector<Triangle*>& triangles, int start, int end, int nextRightNode)
{
	missNext = nextRightNode;
	if (end - start < BVHBuilder::MAX_TRIANGLES_PER_BOX)
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
	nodes[leftInd] = new BVHNodeSAH(nodes, triangles, start, splitIdx, rightInd);
	nodes[rightInd] = new BVHNodeSAH(nodes, triangles, splitIdx + 1, end, nextRightNode);

	box = AABB::getUnitedBox(nodes[leftInd]->box, nodes[rightInd]->box);
}
int BVHNodeSAH::getSplitIndex(std::vector<Triangle*>& triangles, int start, int end)
{
	glm::vec3 min {FLT_MAX}, max {-FLT_MAX};
	for (int i = start; i <= end; i++)
	{
		auto pos = triangles[i]->getCenter();
		min = glm::min(min, pos);
		max = glm::max(max, pos);
	}

	glm::vec3 range = max - min;
	int axis = 0;
	if (range.y > range.x) axis = 1;
	if (range.z > range[axis]) axis = 2;

	std::sort(triangles.begin() + start, triangles.begin() + end + 1, [axis](auto a, auto b)
	{
		return a->getCenter()[axis] < b->getCenter()[axis];
	});


	int bestSplit = start;
	float bestCost = FLT_MAX;

	std::vector leftMin(end - start + 1, glm::vec3(FLT_MAX));
	std::vector leftMax(end - start + 1, glm::vec3(-FLT_MAX));
	std::vector rightMin(end - start + 1, glm::vec3(FLT_MAX));
	std::vector rightMax(end - start + 1, glm::vec3(-FLT_MAX));

	glm::vec3 tempMin = glm::vec3(FLT_MAX), tempMax = glm::vec3(-FLT_MAX);
	for (int i = start; i <= end; i++)
	{
		tempMin = glm::min(tempMin, triangles[i]->getBoundingBox().min_);
		tempMax = glm::max(tempMax, triangles[i]->getBoundingBox().max_);
		leftMin[i - start] = tempMin;
		leftMax[i - start] = tempMax;
	}

	tempMin = glm::vec3(FLT_MAX);
	tempMax = glm::vec3(-FLT_MAX);
	for (int i = end; i >= start; i--)
	{
		tempMin = glm::min(tempMin, triangles[i]->getBoundingBox().min_);
		tempMax = glm::max(tempMax, triangles[i]->getBoundingBox().max_);
		rightMin[i - start] = tempMin;
		rightMax[i - start] = tempMax;
	}

	for (int i = start; i < end; i++)
	{
		int leftCount = i - start + 1;
		int rightCount = end - i;

		glm::vec3 leftSize = leftMax[i - start] - leftMin[i - start];
		glm::vec3 rightSize = rightMax[i - start + 1] - rightMin[i - start + 1];

		float leftArea = 2.0f * (leftSize.x * leftSize.y + leftSize.y * leftSize.z + leftSize.z * leftSize.x);
		float rightArea = 2.0f * (rightSize.x * rightSize.y + rightSize.y * rightSize.z + rightSize.z * rightSize.x);

		float cost = leftCount * leftArea + rightCount * rightArea;
		if (cost < bestCost)
		{
			bestCost = cost;
			bestSplit = i;
		}
	}

	return bestSplit;
}
