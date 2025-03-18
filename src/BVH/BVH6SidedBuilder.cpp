#include "BVH6SidedBuilder.h"
#include "BVHMortonBuilder.h"
#include "Debug.h"
#include "Triangle.h"

static auto& nodes = BVH::nodes;

void BVH6SidedBuilder::buildTree6Sided(const std::vector<Triangle*>& triangles)
{
	//BVHMortonBuilder::build(triangles);

	for (int i = 0; i < 6; i++)
	{
		links6Sided[i].clear();
		links6Sided[i].resize(nodes.size());
		links6Sided[i][0] = buildAxis(i / 2, i % 2 == 1, links6Sided[i], nodes[0]);
	}
}
Link BVH6SidedBuilder::buildAxis(int axis, bool positive, std::vector<Link>& vector, const BVHNode* node, int nextRightNode, int depth)
{
	if (node->isLeaf) return {nextRightNode, nextRightNode};

	auto bound1 = positive ? nodes[node->left]->box.min_ : nodes[node->left]->box.max_;
	auto bound2 = positive ? nodes[node->right]->box.min_ : nodes[node->right]->box.max_;
	if (bound1[axis] < bound2[axis])
	{
		vector[node->left] = buildAxis(axis, positive, vector, nodes[node->left], node->right, depth + 1);
		vector[node->right] = buildAxis(axis, positive, vector, nodes[node->right], nextRightNode, depth + 1);
		return {node->left, nextRightNode};
	}
	else
	{
		vector[node->right] = buildAxis(axis, positive, vector, nodes[node->right], node->left, depth + 1);
		vector[node->left] = buildAxis(axis, positive, vector, nodes[node->left], nextRightNode, depth + 1);
		return {node->right, nextRightNode};
	}
}
