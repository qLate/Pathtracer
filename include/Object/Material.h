#pragma once

#include <filesystem>
#include <vector>

#include "BufferController.h"
#include "Color.h"
#include "JsonUtility.h"
#include "Utils.h"

class GLTexture2D;

class Texture
{
	inline static int _nextAvailableId = 0;

	int _id;
	std::string _path;
	std::string _name;

	float* _data;
	int _width = 0, _height = 0;

	UPtr<GLTexture2D> _glTex;

	bool readImage(std::vector<float>& data_v, const std::filesystem::path& path);
	bool readImageExr(std::vector<float>& data_v, const std::filesystem::path& path);

	void initData(const std::vector<float>& image);

	Texture(const std::filesystem::path& path);
	Texture(const Texture& other);
	Texture() = default;

public:
	Texture(Color color);
	virtual ~Texture();

	static Texture* defaultTex();

	int id() const { return _id; }
	std::string path() const { return _path; }
	std::string name() const { return _name; }
	float* data() const { return _data; }
	int width() const { return _width; }
	int height() const { return _height; }
	UPtr<GLTexture2D>& glTex() { return _glTex; }

	void setName(const std::string& name) { _name = name; }
	void setWrapMode(GLint wrapMode) const;

	Color colorAt(int x, int y) const;

	constexpr static auto properties();

	friend class Assets;
	friend class JsonUtility;
};

class WindyTexture : public Texture
{
	float _scale;
	float _strength;

public:
	WindyTexture(const Color& color, float scale, float strength) : Texture(color), _scale(scale), _strength(strength) {}

	float scale() const { return _scale; }
	float strength() const { return _strength; }

	void setScale(float scale);
	void setStrength(float strength);
};

class Material
{
	inline static int _nextAvailableId = 0;

	int _id;
	bool _lit;
	Color _color;
	Color _specColor = Color::clear();
	Texture* _texture;
	Color _emission;
	float _opacity;
	Texture* _opacityTexture = nullptr;

	float _roughness = 1;
	float _metallic = 0;

public:
	static Material* defaultLit();
	static Material* defaultUnlit();
	static Material* debug();

	Material(Color color, bool lit, Texture* texture, float roughness = 1, float metallic = 0, Color emission = Color::clear(), float opacity = 1,
	         Texture* opacityTexture = nullptr, Color specColor = Color::clear());
	Material(Color color = Color::white(), bool lit = true);
	Material(const Material& material);
	~Material();

	int id() const { return _id; }
	bool lit() const { return _lit; }
	Color color() const { return _color; }
	Color specColor() const { return _specColor; }
	Texture* texture() const { return _texture; }
	float opacity() const { return _opacity; }
	Texture* opacityTexture() const { return _opacityTexture; }
	float roughness() const { return _roughness; }
	float metallic() const { return _metallic; }
	Color emission() const { return _emission; }

	void setLit(bool lit);
	void setColor(const Color& color);
	void setSpecColor(const Color& specColor);
	void setTexture(Texture* texture);
	void setOpacity(float opacity);
	void setOpacityTexture(Texture* texture);
	void setDiffuseCoef(float diffuseCoef);
	void setMetallic(float metallic);
	void setRoughness(float roughness);
	void setEmission(const Color& emission);

	constexpr static auto properties();
};

constexpr auto Texture::properties()
{
	return std::make_tuple(
		JsonUtility::property(&Texture::_path, "path")
	);
}

constexpr auto Material::properties()
{
	return std::tuple_cat(
		std::make_tuple(
			JsonUtility::property(&Material::_lit, "lit"),
			JsonUtility::property(&Material::_color, "color"),
			JsonUtility::property(&Material::_texture, "texture"),
			JsonUtility::property(&Material::_roughness, "diffuseCoef"),
			JsonUtility::property(&Material::_metallic, "metallic"),
			JsonUtility::property(&Material::_emission, "emission")
		)
	);
}
