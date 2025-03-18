#pragma once

#include "BVH.h"

class SSBO;
class ComputeShaderProgram;

class BVHMortonBuilder
{
	static constexpr int TRI_CENTER_ALIGN = 4;
	static constexpr int MORTON_ALIGN = 1;
	static constexpr int MIN_MAX_BOUND_ALIGN = 8;

	inline static ComputeShaderProgram* bvh_part1_morton;
	inline static ComputeShaderProgram* bvh_part2_build;

	inline static SSBO* ssboTriCenters;
	inline static SSBO* ssboMinMaxBound;
	inline static SSBO* ssboMortonCodes;

	static std::vector<std::pair<uint32_t, int>> getSortedTrianglesByMorton(const std::vector<Triangle*>& triangles);
	static void recordOriginalTriIndices(const std::vector<Triangle*>& triangles, const std::vector<std::pair<uint32_t, int>>& sortedCodes);

	static void buildStacked(const std::vector<Triangle*>& triangles, std::vector<std::pair<uint32_t, int>>& sortedCodes);
	static void calcBoxBottomUpForNode(int nodeInd, const std::vector<std::unique_ptr<std::atomic<int>>>& calculated);
	static void calcNodeBoxesParallel(int n);

	static int getSplitIndex_old(const std::vector<std::pair<uint32_t, int>>& sortedIndices, int start, int end);

public:
	static void build(const std::vector<Triangle*>& triangles);

	static void init();
	static void uninit();

	static void buildGPU();
	static void buildGPU_morton(int n);
	static void buildGPU_buildTree(int n);
};
