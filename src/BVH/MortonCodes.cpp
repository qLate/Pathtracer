#include "MortonCodes.h"

#include "glm/common.hpp"
#include <bit>

std::pair<glm::vec3, glm::vec3> MortonCodes::computeBounds(const std::vector<glm::vec3>& points)
{
	glm::vec3 minBound = glm::vec3(std::numeric_limits<float>::max());
	auto maxBound = glm::vec3(std::numeric_limits<float>::lowest());

	for (const auto& p : points)
	{
		minBound = min(minBound, p);
		maxBound = max(maxBound, p);
	}

	return {minBound, maxBound};
}

uint32_t MortonCodes::computeMortonCode(const glm::vec3& point, const glm::vec3& minBound, const glm::vec3& maxBound)
{
	auto normalized = (point - minBound) / (maxBound - minBound);
	auto grid = glm::ivec3(normalized * GRID_RESOLUTION); // 10-bit grid

	return expandBits(grid.x) | expandBits(grid.y) << 1 | expandBits(grid.z) << 2;
}
uint32_t MortonCodes::expandBits(uint32_t x)
{
	x = (x | x << 16) & 0x030000FF;
	x = (x | x << 8) & 0x0300F00F;
	x = (x | x << 4) & 0x030C30C3;
	x = (x | x << 2) & 0x09249249;
	return x;
}

std::vector<uint32_t> MortonCodes::generateMortonCodes(const std::vector<glm::vec3>& centers)
{
	std::vector<uint32_t> mortonCodes(centers.size());

	auto [minBound, maxBound] = computeBounds(centers);
#pragma omp parallel for
	for (int i = 0; i < centers.size(); i++)
		mortonCodes[i] = computeMortonCode(centers[i], minBound, maxBound);

	return mortonCodes;
}

int MortonCodes::commonPrefixLength(uint32_t a, uint32_t b)
{
	return std::countl_zero(a ^ b) - 1;
}
