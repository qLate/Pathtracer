#include "BVHMortonBuilder.h"

#include <execution>

#include "BufferController.h"
#include "GLObject.h"
#include "MortonCodes.h"
#include "RadixSort.hpp"
#include "Renderer.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "Triangle.h"
#include "Utils.h"

static auto& nodes = BVH::nodes;
static auto& originalTriIndices = BVH::originalTriIndices;

void BVHMortonBuilder::build(const std::vector<Triangle*>& triangles)
{
	nodes.clear();

	auto sortedCodes = getSortedTrianglesByMorton(triangles);
	recordOriginalTriIndices(triangles, sortedCodes);

	buildStacked(triangles, sortedCodes);

	buildGPU();
}

void BVHMortonBuilder::init()
{
	bvh_part1_morton = new ComputeShaderProgram("shaders/compute/bvh/bvh_part1_morton.comp");
	bvh_part2_build = new ComputeShaderProgram("shaders/compute/bvh/bvh_part2_build.comp");

	ssboTriCenters = new SSBO(TRI_CENTER_ALIGN);
	ssboMinMaxBound = new SSBO(MIN_MAX_BOUND_ALIGN);
	ssboMortonCodes = new SSBO(MORTON_ALIGN);
	ssboTriIndices = new SSBO(TRI_INDICES_ALIGN);
	ssboBVHNodes = new SSBO(BVH_NODE_ALIGN);

	ssboMinMaxBound->setData(nullptr, 1 * MIN_MAX_BOUND_ALIGN);
}
void BVHMortonBuilder::uninit()
{
	delete bvh_part1_morton;
	delete bvh_part2_build;

	delete ssboTriCenters;
	delete ssboMinMaxBound;
	delete ssboMortonCodes;
	delete ssboTriIndices;
	delete ssboBVHNodes;
}

void BVHMortonBuilder::buildGPU()
{
	int n = Scene::triangles.size();

	buildGPU_morton(n);
	buildGPU_buildTree(n);
}
void BVHMortonBuilder::buildGPU_morton(int n)
{
	ssboTriCenters->bind(6);
	ssboMinMaxBound->bind(7);
	ssboMortonCodes->bind(8);
	ssboTriIndices->bind(9);

	ssboTriCenters->setData(nullptr, n);
	ssboMinMaxBound->clear();
	ssboMortonCodes->setData(nullptr, n);
	ssboTriIndices->setData(nullptr, n);

	BufferController::updateTrianglesBuffer();
	BufferController::updateObjectsBuffer();

	bvh_part1_morton->use();
	bvh_part2_build->setInt("n", n);

	bvh_part1_morton->setInt("pass", 0);
	ComputeShaderProgram::dispatch({n / 256 + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	bvh_part1_morton->setInt("pass", 1);
	ComputeShaderProgram::dispatch({n / 256 + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	glu::RadixSort radix_sort;
	radix_sort(ssboMortonCodes->id, ssboTriIndices->id, n);
}
void BVHMortonBuilder::buildGPU_buildTree(int n)
{
	ssboBVHNodes->bind(6);
	ssboTriIndices->bind(7);
	ssboMortonCodes->bind(8);
	Renderer::renderProgram->fragShader->ssboTriangles->bindDefault();

	ssboBVHNodes->setData(nullptr, 2 * n - 1);

	bvh_part2_build->use();
	bvh_part2_build->setInt("n", n);

	bvh_part2_build->setInt("pass", 0);
	ComputeShaderProgram::dispatch({n / 32 + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	bvh_part2_build->setInt("pass", 1);
	ComputeShaderProgram::dispatch({n / 32 + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	bvh_part2_build->setInt("pass", 2);
	ComputeShaderProgram::dispatch({n / 32 + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	bvh_part2_build->setInt("pass", 3);
	ComputeShaderProgram::dispatch({n / 32 + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	//auto nodes = ssboBVHNodes->readData<BVHNodeStruct>(2 * n - 1);
	//for (int i = 0; i < 2 * n - 1; ++i)
	//{
	//	auto node1Struct = nodes[i];
	//	auto node1 = BVHNode();
	//	node1.left = node1Struct.values.x;
	//	node1.right = node1Struct.values.y;
	//	node1.box = AABB(glm::vec3(node1Struct.min), glm::vec3(node1Struct.max));
	//	node1.isLeaf = node1Struct.values.z == 1;
	//	node1.parent = node1Struct.values.w;
	//	node1.hitNext = node1Struct.links.x;
	//	node1.missNext = node1Struct.links.y;
	//	node1.leafTrianglesStart = node1Struct.min.w;
	//	node1.leafTriangleCount = node1Struct.max.w;

	//	auto node2 = BVH::nodes[i];
	//	int x = -1;
	//}
}

std::vector<std::pair<uint32_t, int>> BVHMortonBuilder::getSortedTrianglesByMorton(const std::vector<Triangle*>& triangles)
{
	auto centers = std::vector<glm::vec3>(triangles.size());
#pragma omp parallel for
	for (int i = 0; i < triangles.size(); i++)
		centers[i] = triangles[i]->getCenter();

	auto mortonCodes = MortonCodes::generateMortonCodes(centers);

	auto sortedCodes = std::vector<std::pair<uint32_t, int>>(triangles.size());
#pragma omp parallel for
	for (int i = 0; i < triangles.size(); i++)
		sortedCodes[i] = {mortonCodes[i], i};

	std::sort(std::execution::par_unseq, sortedCodes.begin(), sortedCodes.end(), [](auto a, auto b) { return a.first < b.first; });
	return sortedCodes;
}

void BVHMortonBuilder::recordOriginalTriIndices(const std::vector<Triangle*>& triangles, const std::vector<std::pair<uint32_t, int>>& sortedCodes)
{
	originalTriIndices.resize(triangles.size());
#pragma omp parallel for
	for (int i = 0; i < triangles.size(); i++)
		originalTriIndices[i] = sortedCodes[i].second;
}

void BVHMortonBuilder::buildStacked(const std::vector<Triangle*>& triangles, std::vector<std::pair<uint32_t, int>>& sortedCodes)
{
	int n = sortedCodes.size();
	auto prevSize = nodes.size();
	nodes.resize(2 * n - 1);
#pragma omp parallel for
	for (int i = prevSize; i < 2 * n - 1; i++)
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
			while (lcp(i, j) == lcp(i, j + 1))
				j++;

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

	calcNodeBoxesParallel(n);
}

void BVHMortonBuilder::calcBoxBottomUpForNode(int nodeInd, const std::vector<std::unique_ptr<std::atomic<int>>>& calculated)
{
	if (calculated[nodeInd]->fetch_add(1) == 0) return;

	auto node = nodes[nodeInd];
	auto left = nodes[node->left];
	auto right = nodes[node->right];
	node->box = AABB::getUnitedBox(left->box, right->box);

	if (node->parent != -1)
		calcBoxBottomUpForNode(node->parent, calculated);
}
void BVHMortonBuilder::calcNodeBoxesParallel(int n)
{
	std::vector<std::unique_ptr<std::atomic<int>>> calculated(n - 1);
#pragma omp parallel for
	for (int i = 0; i < n - 1; i++)
		calculated[i] = std::make_unique<std::atomic<int>>(0);

#pragma omp parallel for
	for (int i = 0; i < n; i++)
		calcBoxBottomUpForNode(nodes[i + n - 1]->parent, calculated);
}

int BVHMortonBuilder::getSplitIndex_old(const std::vector<std::pair<uint32_t, int>>& sortedIndices, int start, int end)
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
