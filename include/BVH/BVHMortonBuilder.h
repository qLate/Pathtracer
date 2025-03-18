#pragma once

#include "BVH.h"

class SSBO;
class ComputeShaderProgram;

class BVHMortonBuilder : public BVHBuilder
{
	static constexpr int TRI_CENTER_ALIGN = 4;
	static constexpr int MORTON_ALIGN = 1;
	static constexpr int MIN_MAX_BOUND_ALIGN = 8;

	inline static ComputeShaderProgram* bvh_part1_morton;
	inline static ComputeShaderProgram* bvh_part2_build;

	inline static SSBO* ssboTriCenters;
	inline static SSBO* ssboMinMaxBound;
	inline static SSBO* ssboMortonCodes;

	static void buildGPU(const std::vector<Triangle*>& triangles);
	static void buildGPU_morton(int n);
	static void buildGPU_buildTree(int n);

	static void buildCPU(const std::vector<Triangle*>& triangles);
	static void buildCPU_morton(const std::vector<Triangle*>& triangles, std::vector<std::pair<uint32_t, int>>& sortedCodes);
	static void buildCPU_recordTriIndices(const std::vector<Triangle*>& triangles, const std::vector<std::pair<uint32_t, int>>& sortedCodes);
	static void buildCPU_buildInternal(const std::vector<Triangle*>& triangles, std::vector<std::pair<uint32_t, int>>& sortedCodes);
	static void buildCPU_buildLeavesAndLinks(int nodeInd, const std::vector<std::unique_ptr<std::atomic<int>>>& calculated);
	static void buildCPU_calcBoxesBottomUp(int n);

public:
	BVHMortonBuilder();
	~BVHMortonBuilder() override;

	void build(const std::vector<Triangle*>& triangles) override;
};
