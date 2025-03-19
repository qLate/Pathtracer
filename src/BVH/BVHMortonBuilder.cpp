#include "BVHMortonBuilder.h"

#include <execution>

#include "BufferController.h"
#include "GLObject.h"
#include "MortonCodes.h"
#include "ShaderProgram.h"
#include "Triangle.h"
#include "Utils.h"

BVHMortonBuilder::BVHMortonBuilder()
{
	_bvhMorton = make_unique<ComputeShaderProgram>("shaders/compute/bvh/bvh_part1_morton.comp");
	_bvhBuild = make_unique<ComputeShaderProgram>("shaders/compute/bvh/bvh_part2_build.comp");
	radixSort = make_unique<glu::RadixSort>();

	_ssboTriCenters = make_unique<SSBO>(TRI_CENTER_ALIGN);
	_ssboMinMaxBound = make_unique<SSBO>(MIN_MAX_BOUND_ALIGN);
	_ssboMortonCodes = make_unique<SSBO>(MORTON_ALIGN);

	_ssboMinMaxBound->setData(nullptr, 1 * MIN_MAX_BOUND_ALIGN);
}

void BVHMortonBuilder::build(const std::vector<Triangle*>& triangles)
{
	buildGPU(triangles);
}

void BVHMortonBuilder::buildGPU(const std::vector<Triangle*>& triangles)
{
	int n = triangles.size();
	buildGPU_morton(n);
	buildGPU_buildTree(n);
}
void BVHMortonBuilder::buildGPU_morton(int n)
{
	_ssboTriCenters->bind(6);
	_ssboMinMaxBound->bind(7);
	_ssboMortonCodes->bind(8);
	BufferController::ssboBVHTriIndices()->bind(9);
	BufferController::ssboBVHNodes()->bind(10);

	_ssboTriCenters->setData(nullptr, n);
	_ssboMinMaxBound->clear();
	_ssboMortonCodes->setData(nullptr, n);
	BufferController::ssboBVHTriIndices()->setData(nullptr, n);
	BufferController::ssboBVHNodes()->setData(nullptr, 2 * n - 1);

	_bvhMorton->use();
	_bvhMorton->setInt("n", n);

	_bvhMorton->setInt("pass", 0);
	ComputeShaderProgram::dispatch({n / SHADER_GROUP_SIZE + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	_bvhMorton->setInt("pass", 1);
	ComputeShaderProgram::dispatch({n / SHADER_GROUP_SIZE + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	radixSort->operator()(_ssboMortonCodes->id(), BufferController::ssboBVHTriIndices()->id(), n);
}
void BVHMortonBuilder::buildGPU_buildTree(int n)
{
	BufferController::ssboBVHNodes()->bind(6);
	BufferController::ssboBVHTriIndices()->bind(7);
	_ssboMortonCodes->bind(8);
	BufferController::ssboTriangles()->bindDefault();

	BufferController::ssboBVHNodes()->setData(nullptr, 2 * n - 1);

	_bvhBuild->use();
	_bvhBuild->setInt("n", n);

	_bvhBuild->setInt("pass", 0);
	ComputeShaderProgram::dispatch({n / SHADER_GROUP_SIZE + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	_bvhBuild->setInt("pass", 1);
	ComputeShaderProgram::dispatch({n / SHADER_GROUP_SIZE + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	_bvhBuild->setInt("pass", 2);
	ComputeShaderProgram::dispatch({n / SHADER_GROUP_SIZE + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	_bvhBuild->setInt("pass", 3);
	ComputeShaderProgram::dispatch({n / SHADER_GROUP_SIZE + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);
}


static auto& nodes = BVH::nodes;
static auto& originalTriIndices = BVH::originalTriIndices;

void BVHMortonBuilder::buildCPU(const std::vector<Triangle*>& triangles)
{
	nodes.clear();

	std::vector<std::pair<uint32_t, int>> sortedCodes;
	buildCPU_morton(triangles, sortedCodes);
	buildCPU_recordTriIndices(triangles, sortedCodes);

	buildCPU_buildInternal(triangles, sortedCodes);
}
void BVHMortonBuilder::buildCPU_morton(const std::vector<Triangle*>& triangles, std::vector<std::pair<uint32_t, int>>& sortedCodes)
{
	auto centers = std::vector<glm::vec3>(triangles.size());
#pragma omp parallel for
	for (int i = 0; i < triangles.size(); i++)
		centers[i] = triangles[i]->getCenter();

	auto mortonCodes = MortonCodes::generateMortonCodes(centers);

	sortedCodes = std::vector<std::pair<uint32_t, int>>(triangles.size());
#pragma omp parallel for
	for (int i = 0; i < triangles.size(); i++)
		sortedCodes[i] = {mortonCodes[i], i};

	std::sort(std::execution::par_unseq, sortedCodes.begin(), sortedCodes.end(), [](auto a, auto b) { return a.first < b.first; });
}

void BVHMortonBuilder::buildCPU_recordTriIndices(const std::vector<Triangle*>& triangles, const std::vector<std::pair<uint32_t, int>>& sortedCodes)
{
	originalTriIndices.resize(triangles.size());
#pragma omp parallel for
	for (int i = 0; i < triangles.size(); i++)
		originalTriIndices[i] = sortedCodes[i].second;
}

void BVHMortonBuilder::buildCPU_buildInternal(const std::vector<Triangle*>& triangles, std::vector<std::pair<uint32_t, int>>& sortedCodes)
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

	buildCPU_calcBoxesBottomUp(n);
}

void BVHMortonBuilder::buildCPU_buildLeavesAndLinks(int nodeInd, const std::vector<std::unique_ptr<std::atomic<int>>>& calculated)
{
	if (calculated[nodeInd]->fetch_add(1) == 0) return;

	auto node = nodes[nodeInd];
	auto left = nodes[node->left];
	auto right = nodes[node->right];
	node->box = AABB::getUnitedBox(left->box, right->box);

	if (node->parent != -1)
		buildCPU_buildLeavesAndLinks(node->parent, calculated);
}
void BVHMortonBuilder::buildCPU_calcBoxesBottomUp(int n)
{
	std::vector<std::unique_ptr<std::atomic<int>>> calculated(n - 1);
#pragma omp parallel for
	for (int i = 0; i < n - 1; i++)
		calculated[i] = std::make_unique<std::atomic<int>>(0);

#pragma omp parallel for
	for (int i = 0; i < n; i++)
		buildCPU_buildLeavesAndLinks(nodes[i + n - 1]->parent, calculated);
}
