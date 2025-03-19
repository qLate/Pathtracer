#pragma once

#include <filesystem>
#include <vector>

#include "Color.h"
#include "Utils.h"

class GLTexture2D;

class Texture
{
	inline static int _nextAvailableId = 0;

	int _id;
	unsigned char* _data;
	int _width = 0, _height = 0;

	UPtr<GLTexture2D> _glTex;

	bool readImage(std::vector<uint8_t>& data_v, const std::filesystem::path& path);
	void setImageData(const std::vector<uint8_t>& image);

public:
	static Texture* defaultTex();

	Texture(const std::filesystem::path& path);

	int id() const { return _id; }
	unsigned char* data() const { return _data; }
	int width() const { return _width; }
	int height() const { return _height; }
	UPtr<GLTexture2D>& glTex() { return _glTex; }
};


class Material
{
	inline static int _nextAvailableId = 0;

	int _id;
	bool _lit;
	Color _color;
	Texture* _texture;

	float _diffuseCoef = 1;
	float _reflection = 0;

public:
	static Material* defaultLit();
	static Material* defaultUnlit();
	static Material* debugLine();

	Material(Color color, bool lit, Texture* texture, float diffuseCoef = 1, float reflection = 0);
	Material(Color color = Color::white(), bool lit = true);
	Material(const Material& material);
	~Material();

	int id() const { return _id; }
	bool lit() const { return _lit; }
	Color color() const { return _color; }
	Texture* texture() const { return _texture; }
	float diffuseCoef() const { return _diffuseCoef; }
	float reflection() const { return _reflection; }

	void setLit(bool lit) { _lit = lit; }
	void setColor(const Color& color) { _color = color; }
	void setTexture(Texture* texture) { _texture = texture; }
	void setDiffuseCoef(float diffuseCoef) { _diffuseCoef = diffuseCoef; }
	void setReflection(float reflection) { _reflection = reflection; }
};
