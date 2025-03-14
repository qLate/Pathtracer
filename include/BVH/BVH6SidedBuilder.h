#pragma once

#include "BVH.h"

class BVH6SidedBuilder
{
	inline static std::vector<std::vector<Link>> links6Sided = std::vector<std::vector<Link>>(6);

	static void buildTree6Sided(const std::vector<Triangle*>& triangles);
	static Link buildAxis(int axis, bool positive, std::vector<Link>& vector, const BVHNode* node, int nextRightNode = -1, int depth = 0);

	friend class BufferController;
};
