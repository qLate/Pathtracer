#pragma once

#include "BVH.h"

class BVHBasicBuilder
{
	class BVHNodeBasic : public BVHNode
	{
	public:
		// Wrong cause sorts original array
		BVHNodeBasic(std::vector<BVHNode*>& nodes, std::vector<Triangle*>& triangles, int start, int end, int nextRightNode = -1);
		static int getSplitIndex(std::vector<Triangle*>& triangles, int start, int end);
	};

	static void buildTreeBasic(std::vector<Triangle*>& triangles);
};
