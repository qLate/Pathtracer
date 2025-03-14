#pragma once

#include "BVH.h"

class BVHSahBuilder
{
	class BVHNodeSAH : BVHNode
	{
	public:
		BVHNodeSAH(std::vector<BVHNode*>& nodes, std::vector<Triangle*>& triangles, int start, int end, int nextRightNode = -1);
		static int getSplitIndex(std::vector<Triangle*>& triangles, int start, int end);
	};

	static void buildTreeSAH(std::vector<Triangle*>& triangles);
};
