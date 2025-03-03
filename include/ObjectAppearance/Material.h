#pragma once

#include <filesystem>
#include <vector>

#include "Color.h"

class Texture
{
	std::filesystem::path path;

	bool readImage(std::vector<uint8_t>& image, const std::filesystem::path& filename);

public:
	static Texture* const DEFAULT_TEX;

	int indexID;

	std::vector<Color> pixelColors;
	int width = 0, height = 0;

	Texture();
	Texture(const std::filesystem::path& path);

	Color getColor(int x, int y) const;
	Color getColor(float u, float v) const;

	int getWidth() const { return width; }
	int getHeight() const { return height; }
	std::filesystem::path getPath() const { return path; }
};


class Material
{
public:
	static Material* const DEBUG_LINE;
	static Material* const DEFAULT_LIT;
	static Material* const DEFAULT_UNLIT;

	int indexID;

	bool lit;
	Color color;
	Texture* texture;

	float diffuseCoeff = 1;
	float specularCoeff = 0;
	float specularDegree = 0;
	float reflection = 0;

	Material(Color color = Color::white(), bool lit = true);
	Material(Color color,
	         bool lit,
	         Texture* texture,
	         float diffuseCoeff,
	         float specularCoeff,
	         float specularDegree,
	         float reflection);
	Material(const Material& material);
	~Material();

	Color getColor(float u, float v) const
	{
		return texture->getColor(u, v) * color;
	}
};
