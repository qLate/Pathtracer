#include "BVHMortonBuilder.h"

#include <execution>

#include "BufferController.h"
#include "GLObject.h"
#include "Graphical.h"
#include "Model.h"
#include "MortonCodes.h"
#include "Renderer.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "Triangle.h"
#include "Utils.h"

BVHMortonBuilder::BVHMortonBuilder()
{
	_bvhMorton = make_unique<ComputeShaderProgram>("shaders/compute/bvh/bvh_part1_morton.comp");
	_bvhBuild = make_unique<ComputeShaderProgram>("shaders/compute/bvh/bvh_part2_build.comp");
	radixSort = make_unique<glu::RadixSort>();

	_ssboCenters = make_unique<SSBO>(TRI_CENTER_ALIGN);
	_ssboMinMaxBound = make_unique<SSBO>(MIN_MAX_BOUND_ALIGN);
	_ssboMortonCodes = make_unique<SSBO>(MORTON_ALIGN);
	_ssboBVHIndices = make_unique<SSBO>(BVH_TRI_INDICES_ALIGN);

	_ssboMinMaxBound->setDataCapacity(1);
}

void BVHMortonBuilder::build()
{
	buildGPU();
}

void BVHMortonBuilder::buildGPU()
{
	int n = Scene::baseTriangles.size();
	auto models = Scene::models;

	int topLevelPrimCount = Scene::graphicals.size();
	int nodeCount = 2 * n - models.size() + 2 * topLevelPrimCount - 1 + 1000;
	BufferController::ssboBVHNodes()->ensureDataCapacity(nodeCount);

	int nodeOffset = 2 * topLevelPrimCount + 1000;
	int primOffset = 0;
	for (int i = 0; i < models.size(); i++)
	{
		auto model = models[i];

		int n_ = model->baseTriangles().size();
		if (n_ < MIN_BOTTOM_LEVEL_TRI_COUNT)
		{
			primOffset += n_;
			continue;
		}
		model->setBvhRootNode(nodeOffset);

		buildCompute_morton(primOffset, n_, false);
		buildCompute_tree(nodeOffset, n_, false);

		nodeOffset += 2 * n_ - 1;
		primOffset += n_;
	}

	BufferController::updateObjects();

	_topLevelStartIndex = 0;
	buildTopLevel();
}

void BVHMortonBuilder::buildTopLevel()
{
	int topLevelPrimCount = Scene::graphicals.size();
	buildCompute_morton(0, topLevelPrimCount, true);
	buildCompute_tree(_topLevelStartIndex, topLevelPrimCount, true);

	BufferController::setBVHRootNode(_topLevelStartIndex);
}

void BVHMortonBuilder::buildCompute_morton(int primOffset, int n_, bool isTopLevel)
{
	_ssboCenters->bind(6);
	_ssboMinMaxBound->bind(7);
	_ssboMortonCodes->bind(8);
	_ssboBVHIndices->bind(9);
	BufferController::ssboBVHNodes()->bind(10);
	BufferController::ssboObjects()->bindDefault();

	_ssboCenters->ensureDataCapacity(n_);
	_ssboMortonCodes->ensureDataCapacity(n_);
	_ssboMinMaxBound->clear();
	_ssboBVHIndices->ensureDataCapacity(n_);

	_bvhMorton->use();
	_bvhMorton->setInt("n", n_);
	_bvhMorton->setInt("primOffset", primOffset);
	_bvhMorton->setBool("isTopLevel", isTopLevel);

	_bvhMorton->setInt("pass", 0);
	ComputeShaderProgram::dispatch({n_ / SHADER_GROUP_SIZE + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	_bvhMorton->setInt("pass", 1);
	ComputeShaderProgram::dispatch({n_ / SHADER_GROUP_SIZE + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	radixSort->operator()(_ssboMortonCodes->id(), _ssboBVHIndices->id(), n_);
}
void BVHMortonBuilder::buildCompute_tree(int nodeOffset, int n_, bool isTopLevel)
{
	BufferController::ssboTriangles()->bindDefault();
	BufferController::ssboObjects()->bindDefault();
	BufferController::ssboBVHNodes()->bind(6);
	_ssboBVHIndices->bind(7);
	_ssboMortonCodes->bind(8);

	_bvhBuild->use();
	_bvhBuild->setInt("n", n_);
	_bvhBuild->setInt("nodeOffset", nodeOffset);
	_bvhBuild->setBool("isTopLevel", isTopLevel);

	_bvhBuild->setInt("pass", 0);
	ComputeShaderProgram::dispatch({(2 * n_ - 1) / SHADER_GROUP_SIZE + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	_bvhBuild->setInt("pass", 1);
	ComputeShaderProgram::dispatch({(2 * n_ - 1) / SHADER_GROUP_SIZE + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	_bvhBuild->setInt("pass", 2);
	ComputeShaderProgram::dispatch({(2 * n_ - 1) / SHADER_GROUP_SIZE + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);

	_bvhBuild->setInt("pass", 3);
	ComputeShaderProgram::dispatch({(2 * n_ - 1) / SHADER_GROUP_SIZE + 1, 1, 1}, GL_SHADER_STORAGE_BARRIER_BIT);
}

//
//static auto& nodes = BVH::nodes;
//static auto& originalTriIndices = BVH::originalTriIndices;
//
//void BVHMortonBuilder::buildCPU(const std::vector<Triangle*>& triangles)
//{
//	nodes.clear();
//
//	std::vector<std::pair<uint32_t, int>> sortedCodes;
//	buildCPU_morton(triangles, sortedCodes);
//	buildCPU_recordTriIndices(triangles, sortedCodes);
//	buildCPU_buildInternal(triangles, sortedCodes);
//}
//void BVHMortonBuilder::buildCPU_morton(const std::vector<Triangle*>& triangles, std::vector<std::pair<uint32_t, int>>& sortedCodes)
//{
//	auto centers = std::vector<glm::vec3>(triangles.size());
//#pragma omp parallel for
//	for (int i = 0; i < triangles.size(); i++)
//		centers[i] = triangles[i]->getCenter();
//
//	auto mortonCodes = MortonCodes::generateMortonCodes(centers);
//
//	sortedCodes = std::vector<std::pair<uint32_t, int>>(triangles.size());
//#pragma omp parallel for
//	for (int i = 0; i < triangles.size(); i++)
//		sortedCodes[i] = {mortonCodes[i], i};
//
//	std::sort(std::execution::par_unseq, sortedCodes.begin(), sortedCodes.end(), [](auto a, auto b) { return a.first < b.first; });
//}
//
//void BVHMortonBuilder::buildCPU_recordTriIndices(const std::vector<Triangle*>& triangles, const std::vector<std::pair<uint32_t, int>>& sortedCodes)
//{
//	originalTriIndices.resize(triangles.size());
//#pragma omp parallel for
//	for (int i = 0; i < triangles.size(); i++)
//		originalTriIndices[i] = sortedCodes[i].second;
//}
//
//void BVHMortonBuilder::buildCPU_buildInternal(const std::vector<Triangle*>& triangles, std::vector<std::pair<uint32_t, int>>& sortedCodes)
//{
//	int n = sortedCodes.size();
//	auto prevSize = nodes.size();
//	nodes.resize(2 * n - 1);
//#pragma omp parallel for
//	for (int i = prevSize; i < 2 * n - 1; i++)
//		nodes[i] = new BVHNode();
//
//	auto lcp = [&sortedCodes, &n](int i, int j)
//	{
//		if (i < 0 || i >= n || j < 0 || j >= n) return -1;
//		return std::countl_zero(sortedCodes[i].first ^ sortedCodes[j].first);
//	};
//
//#pragma omp parallel for
//	for (int i = 0; i < n - 1; i++)
//	{
//		int dir = glm::sign(lcp(i, i + 1) - lcp(i, i - 1));
//		int minPrefix = lcp(i, i - dir);
//
//		int l_max = 2;
//		while (lcp(i, i + l_max * dir) > minPrefix)
//			l_max *= 2;
//
//		int l = 0;
//		for (int step = l_max / 2; step > 0; step /= 2)
//		{
//			if (lcp(i, i + (l + step) * dir) > minPrefix)
//				l += step;
//		}
//		int j = i + l * dir;
//
//		// Handle duplicate codes
//		if (dir == 0)
//			while (lcp(i, j) == lcp(i, j + 1))
//				j++;
//
//		int split;
//		if (sortedCodes[i].first == sortedCodes[j].first)
//			split = std::min(i, j);
//		else
//		{
//			int delta = lcp(i, j);
//			int s = 0;
//			int t = abs(j - i);
//			for (int div = 2; t * 2 / div > 0; div *= 2)
//			{
//				int t_ = ceil(t / (float)div);
//				if (lcp(i, i + (s + t_) * dir) > delta)
//					s += t_;
//			}
//			split = i + s * dir + std::min(dir, 0);
//		}
//
//		auto left = std::min(i, j) == split ? split + (n - 1) : split;
//		auto right = std::max(i, j) == split + 1 ? split + 1 + (n - 1) : split + 1;
//
//		nodes[i]->left = left;
//		nodes[i]->right = right;
//		nodes[left]->parent = i;
//		nodes[right]->parent = i;
//	}
//
//#pragma omp parallel for
//	for (int i = 0; i < n - 1; i++)
//	{
//		int left = nodes[i]->left;
//		int right = nodes[i]->right;
//
//		nodes[i]->hitNext = left;
//		nodes[left]->missNext = right;
//
//		if (nodes[i]->parent != -1)
//		{
//			int curr = right;
//			do
//			{
//				curr = nodes[curr]->parent;
//				if (nodes[curr]->parent == -1)
//				{
//					nodes[right]->missNext = -1;
//					break;
//				}
//				nodes[right]->missNext = nodes[nodes[curr]->parent]->right;
//			}
//			while (nodes[right]->missNext == curr);
//		}
//
//		if (left >= n - 1) nodes[left]->setLeaf([&triangles, &sortedCodes](int k) { return triangles[sortedCodes[k].second]; }, left - (n - 1), left - (n - 1));
//		if (right >= n - 1) nodes[right]->setLeaf([&triangles, &sortedCodes](int k) { return triangles[sortedCodes[k].second]; }, right - (n - 1), right - (n - 1));
//	}
//
//	buildCPU_calcBoxesBottomUp(n);
//}
//
//void BVHMortonBuilder::buildCPU_buildLeavesAndLinks(int nodeInd, const std::vector<std::unique_ptr<std::atomic<int>>>& calculated)
//{
//	if (calculated[nodeInd]->fetch_add(1) == 0) return;
//
//	auto node = nodes[nodeInd];
//	auto left = nodes[node->left];
//	auto right = nodes[node->right];
//	node->box = AABB::getUnitedBox(left->box, right->box);
//
//	if (node->parent != -1)
//		buildCPU_buildLeavesAndLinks(node->parent, calculated);
//}
//void BVHMortonBuilder::buildCPU_calcBoxesBottomUp(int n)
//{
//	std::vector<std::unique_ptr<std::atomic<int>>> calculated(n - 1);
//#pragma omp parallel for
//	for (int i = 0; i < n - 1; i++)
//		calculated[i] = std::make_unique<std::atomic<int>>(0);
//
//#pragma omp parallel for
//	for (int i = 0; i < n; i++)
//		buildCPU_buildLeavesAndLinks(nodes[i + n - 1]->parent, calculated);
//}
