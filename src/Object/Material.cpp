#include "Material.h"

#include <stb_image.h>
#include <tinyexr.h>

#include "BufferController.h"
#include "Scene.h"
#include "Utils.h"

Texture::Texture(const std::filesystem::path& path) : _id(_nextAvailableId++), _path(path.string())
{
	Scene::textures.push_back(this);

	std::vector<uint8_t> image;
	if (!readImage(image, path))
	{
		Debug::logError("Error loading texture: ", path);
		return;
	}
	initData(image);

	_glTex = std::make_unique<GLTexture2D>(_width, _height, _data);

	BufferController::markBufferForUpdate(BufferType::Textures);
}
Texture::Texture(Color color) : _id(_nextAvailableId++)
{
	Scene::textures.push_back(this);

	_width = 2;
	_height = 2;

	_data = new unsigned char[4 * sizeof(Color)];

	auto r = Utils::toByte(color.x);
	auto g = Utils::toByte(color.y);
	auto b = Utils::toByte(color.z);
	auto a = Utils::toByte(color.w);

	for (int i = 0; i < _width * _height; ++i)
	{
		_data[i * 4 + 0] = r;
		_data[i * 4 + 1] = g;
		_data[i * 4 + 2] = b;
		_data[i * 4 + 3] = a;
	}

	_glTex = std::make_unique<GLTexture2D>(_width, _height, _data);

	BufferController::markBufferForUpdate(BufferType::Textures);
}
Texture::Texture(const Texture& other) : Texture(other._path) {}

Texture::~Texture()
{
	delete[] _data;
	std::erase(Scene::textures, this);
}

Texture* Texture::defaultTex()
{
	static Texture instance("assets/textures/core/default.png");
	return &instance;
}

bool Texture::readImage(std::vector<uint8_t>& data_v, const std::filesystem::path& path)
{
	int n;
	if (path.extension() == ".exr")
		return readImageExr(data_v, path);

	unsigned char* data = stbi_load(path.string().c_str(), &_width, &_height, &n, STBI_rgb_alpha);

	if (data != nullptr)
		data_v = std::vector(data, data + _width * _height * 4);

	stbi_image_free(data);
	return true;
}
bool Texture::readImageExr(std::vector<uint8_t>& data_v, const std::filesystem::path& path)
{
	float* exrData = nullptr;
	const char* err;

	int ret = LoadEXR(&exrData, &_width, &_height, path.string().c_str(), &err);

	if (ret != TINYEXR_SUCCESS && err)
	{
		fprintf(stderr, "ERR : %s\n", err);
		FreeEXRErrorMessage(err);

		return false;
	}

	data_v.resize(_width * _height * 4);
	for (int i = 0; i < _width * _height; ++i)
	{
		float r = exrData[4 * i + 0];
		float g = exrData[4 * i + 1];
		float b = exrData[4 * i + 2];
		float a = exrData[4 * i + 3];

		r = powf(glm::clamp(r, 0.0f, 1.0f), 1.0f / 2.2f);
		g = powf(glm::clamp(g, 0.0f, 1.0f), 1.0f / 2.2f);
		b = powf(glm::clamp(b, 0.0f, 1.0f), 1.0f / 2.2f);
		a = glm::clamp(a, 0.0f, 1.0f);

		data_v[4 * i + 0] = r * 255.0f;
		data_v[4 * i + 1] = g * 255.0f;
		data_v[4 * i + 2] = b * 255.0f;
		data_v[4 * i + 3] = a * 255.0f;
	}

	return true;
}

void Texture::initData(const std::vector<uint8_t>& image)
{
	_data = new unsigned char[_width * _height * 4];
	memcpy(_data, image.data(), _width * _height * 4);
}

void WindyTexture::setScale(float scale)
{
	_scale = scale;
	BufferController::markBufferForUpdate(BufferType::Materials);
}
void WindyTexture::setStrength(float strength)
{
	_strength = strength;
	BufferController::markBufferForUpdate(BufferType::Materials);
}

Material* Material::defaultLit()
{
	static Material instance(Color::gray(), true);
	return &instance;
}
Material* Material::defaultUnlit()
{
	static Material instance(Color::white(), false);
	return &instance;
}
Material* Material::debug()
{
	static Material instance(Color::red(), false);
	return &instance;
}

Material::Material(Color color, bool lit, Texture* texture, float roughness, float metallic, Color emission, float opacity, Texture* opacityTexture, Color specColor) :
	_id(_nextAvailableId++), _lit{lit}, _color{color}, _specColor{specColor}, _texture{texture}, _emission{emission}, _opacity{opacity}, _opacityTexture{opacityTexture},
	_roughness{roughness}, _metallic{metallic}
{
	Scene::materials.push_back(this);

	BufferController::markBufferForUpdate(BufferType::Materials);
}
Material::Material(Color color, bool lit) : Material(color, lit, Texture::defaultTex()) {}
Material::Material(const Material& material) : Material(material._color, material._lit, material._texture, material._roughness, material._metallic, material._emission) {}
Material::~Material()
{
	std::erase(Scene::materials, this);
}

void Material::setLit(bool lit)
{
	_lit = lit;

	BufferController::markBufferForUpdate(BufferType::Materials);
}
void Material::setColor(const Color& color)
{
	_color = color;

	BufferController::markBufferForUpdate(BufferType::Materials);
}
void Material::setSpecColor(const Color& specColor)
{
	_emission = specColor;

	BufferController::markBufferForUpdate(BufferType::Materials);
}
void Material::setTexture(Texture* texture)
{
	_texture = texture;

	BufferController::markBufferForUpdate(BufferType::Materials);
	BufferController::markBufferForUpdate(BufferType::Textures);
}
void Material::setOpacity(float opacity)
{
	_opacity = opacity;

	BufferController::markBufferForUpdate(BufferType::Materials);
}
void Material::setOpacityTexture(Texture* texture)
{
	_opacityTexture = texture;

	BufferController::markBufferForUpdate(BufferType::Materials);
	BufferController::markBufferForUpdate(BufferType::Textures);
}
void Material::setDiffuseCoef(float diffuseCoef)
{
	_roughness = diffuseCoef;

	BufferController::markBufferForUpdate(BufferType::Materials);
}
void Material::setMetallic(float metallic)
{
	_metallic = metallic;

	BufferController::markBufferForUpdate(BufferType::Materials);
}
void Material::setRoughness(float roughness)
{
	_roughness = roughness;

	BufferController::markBufferForUpdate(BufferType::Materials);
}
void Material::setEmission(const Color& emission)
{
	_emission = emission;

	BufferController::markBufferForUpdate(BufferType::Materials);
	BufferController::markBufferForUpdate(BufferType::Lights);
}
