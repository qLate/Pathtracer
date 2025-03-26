#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <ostream>
#include <glm/common.hpp>
#include <glm/vec4.hpp>

class Color : public glm::vec4
{
public:
	Color(float r, float g, float b, float a = 1) : glm::vec4(r, g, b, a) {}
	Color(const glm::vec4& v) : Color(v.x, v.y, v.z, v.w) {}
	Color() : Color(0, 0, 0, 0) {}

	static Color white() { return {1.f, 1.f, 1.f}; }
	static Color black() { return {0.f, 0.f, 0.f}; }
	static Color red() { return {1.f, 0.f, 0.f}; }
	static Color green() { return {0.f, 1.f, 0.f}; }
	static Color blue() { return {0.f, 0.f, 1.f}; }
	static Color yellow() { return {1.f, 1.f, 0.f}; }
	static Color cyan() { return {0.f, 1.f, 1.f}; }
	static Color magenta() { return {1.f, 0.f, 1.f}; }
	static Color gray() { return {0.2f, 0.2f, 0.2f}; }
	static Color darkGray() { return {0.1f, 0.1f, 0.1f}; }

	static Color darkGreen() { return {0.f, 0.5f, 0.f}; }
	static Color skyblue() { return {.529f, .808f, .922f}; }
	static Color gold() { return {1.f, .843f, 0.f}; }

	float r() const { return x; }
	float g() const { return y; }
	float b() const { return z; }
	float a() const { return w; }

	float intensity() const { return (r() + g() + b()) / 3; }
	Color withIntensity(float intensity) const
	{
		if (this->intensity() == 0) return white() * intensity;
		return *this * intensity / this->intensity();
	}

	Color& operator*=(float v);
	Color& operator*=(Color c);

	Color operator*(Color c) const { return {x * c.x, y * c.y, z * c.z}; }

	Color& operator+=(Color c);
	Color operator+(Color c) const { return {x + c.x, y + c.y, z + c.z}; }

	Color operator-=(Color c);
	Color operator-(Color c) const { return {x - c.x, y - c.y, z - c.z}; }
	Color operator-() const { return {-x, -y, -z}; }

	uint32_t toColor32() const;
	static Color fromColor32(uint32_t color32);

	static Color lerp(const Color& c1, const Color& c2, float value);
};

inline Color& Color::operator*=(float v)
{
	x *= v;
	y *= v;
	z *= v;
	return *this;
}
inline Color& Color::operator*=(Color c)
{
	x *= c.x;
	y *= c.y;
	z *= c.z;
	return *this;
}
inline Color& Color::operator+=(Color c)
{
	x += c.x;
	y += c.y;
	z += c.z;
	return *this;
}
inline Color Color::operator-=(Color c)
{
	x -= c.x;
	y -= c.y;
	z -= c.z;
	return *this;
}

inline uint32_t Color::toColor32() const
{
	return ((int)(glm::clamp(a(), 0.0f, 1.0f) * 255) << 24) +
		((int)(glm::clamp(r(), 0.0f, 1.0f) * 255) << 16) +
		((int)(glm::clamp(g(), 0.0f, 1.0f) * 255) << 8) +
		((int)(glm::clamp(b(), 0.0f, 1.0f) * 255) << 0);
}
inline Color Color::fromColor32(uint32_t color32)
{
	auto a = (float)(color32 >> 24 & 0xFF) / 255;
	auto r = (float)(color32 >> 16 & 0xFF) / 255;
	auto g = (float)(color32 >> 8 & 0xFF) / 255;
	auto b = (float)(color32 >> 0 & 0xFF) / 255;
	return {r, g, b, a};
}
inline Color Color::lerp(const Color& c1, const Color& c2, float value)
{
	auto r = c1.r() + (c2.r() - c1.r()) * value;
	auto g = c1.g() + (c2.g() - c1.g()) * value;
	auto b = c1.b() + (c2.b() - c1.b()) * value;
	return {r, g, b};
}


inline Color operator*(float v, Color c) { return {c.x * v, c.y * v, c.z * v}; }
inline Color operator*(Color c, float v) { return {c.x * v, c.y * v, c.z * v}; }

inline std::ostream& operator<<(std::ostream& os, const Color& c)
{
	os << "(" << c.r() << ", " << c.g() << ", " << c.b() << ")";
	return os;
}
