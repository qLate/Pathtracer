#pragma once

#include <string>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/gtx/dual_quaternion.hpp"

constexpr float PI = 3.14159265358979323846f;
constexpr float DEG_TO_RAD = 1 / (180 / PI);
constexpr float RAD_TO_DEG = 180 / PI;

enum class EaseType
{
	Linear = 0,
	InQuad = 1,
	OutQuad = 2,
	InOutQuad = 3,
	InCubic = 4,
	OutCubic = 5,
	InOutCubic = 6,
	InQuart = 7,
	OutQuart = 8,
	InOutQuart = 9,
};

namespace vec2
{
	inline static constexpr glm::vec2 ZERO = {0, 0};
	inline static constexpr glm::vec2 ONE = {1, 1};
}

namespace vec3
{
	inline static constexpr glm::vec3 ZERO = {0, 0, 0};
	inline static constexpr glm::vec3 ONE = {1, 1, 1};
	inline static constexpr glm::vec3 UP = {0, 1, 0};
	inline static constexpr glm::vec3 DOWN = {0, -1, 0};
	inline static constexpr glm::vec3 FORWARD = {0, 0, 1};
	inline static constexpr glm::vec3 BACKWARD = {0, 0, -1};
	inline static constexpr glm::vec3 LEFT = {-1, 0, 0};
	inline static constexpr glm::vec3 RIGHT = {1, 0, 0};
}

class Math
{
public:
	static float det(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
	static bool solveQuadratic(float a, float b, float c, float& x0, float& x1);
	static float getLengthSquared(glm::vec3 v);

	static float random(float min, float max);

	static glm::vec3 randomVectorInCircle(float radius);
	static glm::vec3 randomVectorInRectangle(float sizeX, float sizeY);
	static glm::vec3 randomVectorInSphere(float radius); // Wrong!!!
	static glm::vec3 randomDirection();

	static float evaluateEase(EaseType ease, float t);

	static std::string toString(const glm::vec3& v, int precision = 2);
	static std::string toString(const glm::vec2& v, int precision = 2);
};
