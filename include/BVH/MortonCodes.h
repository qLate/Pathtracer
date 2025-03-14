#pragma once

#include <vector>

#include "glm/vec3.hpp"

class MortonCodes
{
	static constexpr int GRID_DEPTH = 10;
	inline static const float GRID_RESOLUTION = powf(2, GRID_DEPTH) - 1;

	static std::pair<glm::vec3, glm::vec3> computeBounds(const std::vector<glm::vec3>& points);

	static uint32_t computeMortonCode(const glm::vec3& point, const glm::vec3& minBound, const glm::vec3& maxBound);
	static uint32_t expandBits(uint32_t x);

public:
	static std::vector<uint32_t> generateMortonCodes(const std::vector<glm::vec3>& centers);
	static int commonPrefixLength(uint32_t a, uint32_t b);

	friend class BVHNodeMorton;
};
