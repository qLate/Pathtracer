#include "MyMath.h"

#include <stdexcept>

float Math::det(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
	const float x = v2[1] * v3[2] - v3[1] * v2[2];
	const float y = v2[0] * v3[2] - v3[0] * v2[2];
	const float z = v2[0] * v3[1] - v3[0] * v2[1];

	return v1[0] * x - v1[1] * y + v1[2] * z;
}
bool Math::solveQuadratic(float a, float b, float c, float& x0, float& x1)
{
	float discr = b * b - 4 * a * c;
	if (discr < 0) return false;
	if (discr == 0)
		x0 = x1 = -0.5f * b / a;
	else
	{
		float q = b > 0 ? -0.5f * (b + sqrt(discr)) : -0.5f * (b - sqrt(discr));
		x0 = q / a;
		x1 = c / q;
	}
	if (x0 > x1)
		std::swap(x0, x1);
	return true;
}

float Math::random(float min, float max)
{
	return min + static_cast<float>(rand()) / (RAND_MAX / (max - min));
}

float Math::getLengthSquared(glm::vec3 v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

glm::vec3 Math::randomVectorInCircle(float radius)
{
	while (true)
	{
		auto p = glm::vec3(random(-radius, radius), random(-radius, radius), 0);
		if (getLengthSquared(p) >= 1) continue;
		return p;
	}
}
glm::vec3 Math::randomVectorInRectangle(float sizeX, float sizeY)
{
	return { random(-sizeX, sizeX), random(-sizeY, sizeY), 0 };
}
glm::vec3 Math::randomVectorInSphere(float radius)
{
	while (true)
	{
		auto p = glm::vec3(random(-radius, radius), random(-radius, radius), random(-radius, radius));
		if (getLengthSquared(p) >= 1) continue;
		return p;
	}
}
glm::vec3 Math::randomDirection()
{
	return normalize(randomVectorInSphere(1));
}

float Math::evaluateEase(EaseType ease, float t)
{
	switch (ease)
	{
	case EaseType::Linear: return t;
	case EaseType::InQuad: return t * t;
	case EaseType::InCubic: return t * t * t;
	case EaseType::InQuart: return t * t * t * t;
	case EaseType::OutQuad: return 1 - (float)pow(1 - t, 2);
	case EaseType::OutCubic: return 1 - (float)pow(1 - t, 3);
	case EaseType::OutQuart: return 1 - (float)pow(1 - t, 4);
	case EaseType::InOutQuad: return t < 0.5 ? 2 * t * t : 1 - (float)pow(-2 * t + 2, 2) / 2;
	case EaseType::InOutCubic: return t < 0.5 ? 4 * t * t * t : 1 - (float)pow(-2 * t + 2, 3) / 2;
	case EaseType::InOutQuart: return t < 0.5 ? 8 * t * t * t * t : 1 - (float)pow(-2 * t + 2, 4) / 2;
	}

	throw std::runtime_error("Unknown ease type.");
}

std::string Math::toString(const glm::vec3& v, int precision)
{
	auto mult = pow(10, precision);
	return "(" + std::to_string(round(v.x * mult) / mult) + ", " + std::to_string(round(v.y * mult) / mult) + ", " + std::to_string(round(v.z * mult) / mult) + ")";
}
std::string Math::toString(const glm::vec2& v, int precision)
{
	auto mult = pow(10, precision);
	return "(" + std::to_string(round(v.x * mult) / mult) + ", " + std::to_string(round(v.y * mult) / mult) + ")";
}