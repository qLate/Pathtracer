#pragma once

#include "BVH.h"

class BVHMortonBuilder
{
	static std::vector<std::pair<uint32_t, int>> getSortedTrianglesByMorton(const std::vector<Triangle*>& triangles);
	static void recordOriginalTriIndices(const std::vector<Triangle*>& triangles, const std::vector<std::pair<uint32_t, int>>& sortedTriangleIndices);

	static void buildStacked(const std::vector<Triangle*>& triangles, std::vector<std::pair<uint32_t, int>> sortedTriangleIndices);
	static int getSplitIndex(const std::vector<std::pair<uint32_t, int>>& sortedIndices, int start, int end);
	static void calculateBoxes();

public:
	static void build(const std::vector<Triangle*>& triangles);
};
