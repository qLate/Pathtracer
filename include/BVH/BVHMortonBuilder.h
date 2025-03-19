#pragma once

#include "BVH.h"
#include "GLObject.h"
#include "ShaderProgram.h"

class BVHMortonBuilder : public BVHBuilder
{
	static constexpr int TRI_CENTER_ALIGN = 4;
	static constexpr int MORTON_ALIGN = 1;
	static constexpr int MIN_MAX_BOUND_ALIGN = 8;

	inline static UPtr<ComputeShaderProgram> _bvhMorton;
	inline static UPtr<ComputeShaderProgram> _bvhBuild;

	inline static UPtr<SSBO> _ssboTriCenters;
	inline static UPtr<SSBO> _ssboMinMaxBound;
	inline static UPtr<SSBO> _ssboMortonCodes;

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

	void build(const std::vector<Triangle*>& triangles) override;
};
