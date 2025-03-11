#pragma once

class MortonCodes
{
public:
	static uint32_t expandBits(uint32_t x)
	{
		x = (x | x << 16) & 0x030000FF;
		x = (x | x << 8) & 0x0300F00F;
		x = (x | x << 4) & 0x030C30C3;
		x = (x | x << 2) & 0x09249249;
		return x;
	}

	inline static const float GRID_RESOLUTION = powf(2, 10) - 1;
	static uint32_t computeMortonCode(const glm::vec3& point, const glm::vec3& minBound, const glm::vec3& maxBound)
	{
		auto normalized = (point - minBound) / (maxBound - minBound);
		auto grid = glm::ivec3(normalized * GRID_RESOLUTION); // 10-bit grid

		return expandBits(grid.x) | expandBits(grid.y) << 1 | expandBits(grid.z) << 2;
	}

	static std::pair<glm::vec3, glm::vec3> computeBounds(const std::vector<glm::vec3>& points)
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

	static std::vector<uint32_t> generateMortonCodes(const std::vector<glm::vec3>& centers)
	{
		std::vector<uint32_t> mortonCodes;
		mortonCodes.reserve(centers.size());

		auto [minBound, maxBound] = computeBounds(centers);
		for (const auto& center : centers)
			mortonCodes.push_back(computeMortonCode(center, minBound, maxBound));

		return mortonCodes;
	}
};
