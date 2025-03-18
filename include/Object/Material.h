#pragma once

#include <filesystem>
#include <vector>

#include "Color.h"
#include "Utils.h"

class GLTexture2D;

class Texture
{
	bool readImage(std::vector<uint8_t>& data_v, const std::filesystem::path& path);
	void setImageData(const std::vector<uint8_t>& image);

public:
	static Texture* defaultTex();

	unsigned char* data;
	int width = 0, height = 0;

	int texArrayLayerIndex = -1;

	Texture(const std::filesystem::path& path);
};


class Material
{
	inline static int nextAvailableId = 0;

public:
	static Material* defaultLit();
	static Material* defaultUnlit();
	static Material* debugLine();

	int id;

	bool lit;
	Color color;
	Texture* texture;

	float diffuseCoef = 1;
	float reflection = 0;

	Material(Color color, bool lit, Texture* texture, float diffuseCoef = 1, float reflection = 0);
	Material(Color color = Color::white(), bool lit = true);
	Material(const Material& material);
	~Material();
};
