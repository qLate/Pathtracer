#include "BVHMortonBuilder.h"

#include <algorithm>
#include <bit>

#include "MortonCodes.h"
#include "Triangle.h"
#include "Utils.h"
#include "omp.h"

static auto& nodes = BVH::nodes;
static auto& originalTriIndices = BVH::originalTriIndices;

void BVHMortonBuilder::build(const std::vector<Triangle*>& triangles)
{
	nodes.clear();

	auto sortedCodes = getSortedTrianglesByMorton(triangles);
	recordOriginalTriIndices(triangles, sortedCodes);

	buildStacked(triangles, sortedCodes);
	//calculateBoxes();
}

std::vector<std::pair<uint32_t, int>> BVHMortonBuilder::getSortedTrianglesByMorton(const std::vector<Triangle*>& triangles)
{
	auto centers = std::vector<glm::vec3>(triangles.size());
	for (int i = 0; i < triangles.size(); i++)
		centers[i] = triangles[i]->getCenter();

	auto mortonCodes = MortonCodes::generateMortonCodes(centers);

	auto sortedCodes = std::vector<std::pair<uint32_t, int>>(triangles.size());
	for (int i = 0; i < triangles.size(); i++)
		sortedCodes[i] = {mortonCodes[i], i};

	std::ranges::sort(sortedCodes, [](auto a, auto b) { return a.first < b.first; });
	return sortedCodes;
}

void BVHMortonBuilder::recordOriginalTriIndices(const std::vector<Triangle*>& triangles, const std::vector<std::pair<uint32_t, int>>& sortedCodes)
{
	originalTriIndices.resize(triangles.size());
	for (int i = 0; i < triangles.size(); i++)
		originalTriIndices[i] = sortedCodes[i].second;
}

AABB calcNodeBoxes(int node)
{
	auto node_ = nodes[node];
	if (node_->isLeaf)
		return node_->box;

	auto leftBox = calcNodeBoxes(node_->left);
	auto rightBox = calcNodeBoxes(node_->right);
	auto box = AABB::getUnitedBox(leftBox, rightBox);
	node_->box = box;
	return box;
}

bool compareCodes(const std::pair<uint32_t, int>& a, const std::pair<uint32_t, int>& b)
{
	return a.first < b.first;
}

void BVHMortonBuilder::buildStacked(const std::vector<Triangle*>& triangles, std::vector<std::pair<uint32_t, int>>& sortedCodes)
{
	int n = sortedCodes.size();
	nodes.resize(2 * n - 1);
#pragma omp parallel for
	for (int i = 0; i < 2 * n - 1; i++)
		nodes[i] = new BVHNode();

	auto lcp = [&sortedCodes, &n](int i, int j)
	{
		if (i < 0 || i >= n || j < 0 || j >= n) return -1;
		return std::countl_zero(sortedCodes[i].first ^ sortedCodes[j].first);
	};

#pragma omp parallel for
	for (int i = 0; i < n - 1; i++)
	{
		int dir = glm::sign(lcp(i, i + 1) - lcp(i, i - 1));
		int minPrefix = lcp(i, i - dir);

		int l_max = 2;
		while (lcp(i, i + l_max * dir) > minPrefix)
			l_max *= 2;

		int l = 0;
		for (int step = l_max / 2; step > 0; step /= 2)
		{
			if (lcp(i, i + (l + step) * dir) > minPrefix)
				l += step;
		}
		int j = i + l * dir;

		// Handle duplicate codes
		if (dir == 0)
		{
			while (lcp(i, j) == lcp(i, j + 1))
				j++;
		}

		int split;
		if (sortedCodes[i].first == sortedCodes[j].first)
			split = std::min(i, j);
		else
		{
			int delta = lcp(i, j);
			int s = 0;
			int t = abs(j - i);
			for (int div = 2; t * 2 / div > 0; div *= 2)
			{
				int t_ = ceil(t / (float)div);
				if (lcp(i, i + (s + t_) * dir) > delta)
					s += t_;
			}
			split = i + s * dir + std::min(dir, 0);
		}

		auto left = std::min(i, j) == split ? split + (n - 1) : split;
		auto right = std::max(i, j) == split + 1 ? split + 1 + (n - 1) : split + 1;

		nodes[i]->left = left;
		nodes[i]->right = right;
		nodes[left]->parent = i;
		nodes[right]->parent = i;
	}

#pragma omp parallel for
	for (int i = 0; i < n - 1; i++)
	{
		int left = nodes[i]->left;
		int right = nodes[i]->right;

		nodes[i]->hitNext = left;
		nodes[left]->missNext = right;

		if (nodes[i]->parent != -1)
		{
			int curr = right;
			do
			{
				curr = nodes[curr]->parent;
				if (nodes[curr]->parent == -1)
				{
					nodes[right]->missNext = -1;
					break;
				}
				nodes[right]->missNext = nodes[nodes[curr]->parent]->right;
			}
			while (nodes[right]->missNext == curr);
		}

		if (left >= n - 1) nodes[left]->setLeaf([&triangles, &sortedCodes](int k) { return triangles[sortedCodes[k].second]; }, left - (n - 1), left - (n - 1));
		if (right >= n - 1) nodes[right]->setLeaf([&triangles, &sortedCodes](int k) { return triangles[sortedCodes[k].second]; }, right - (n - 1), right - (n - 1));
	}

	calcNodeBoxes(0);
}

void BVHMortonBuilder::calculateBoxes()
{
	for (int i = nodes.size() - 1; i >= 0; i--)
	{
		auto node = nodes[i];
		if (node->isLeaf) continue;

		auto left = nodes[node->left];
		auto right = nodes[node->right];
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
