#include "Material.h"

#include <stb_image.h>

#include "BufferController.h"
#include "Scene.h"
#include "Utils.h"

Texture::Texture(const std::filesystem::path& path) : _id(_nextAvailableId++), _path(path.string())
{
	Scene::textures.push_back(this);

	std::vector<uint8_t> image;
	if (!readImage(image, path)) Debug::logError("Error loading texture: ", path);
	initData(image);

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
	unsigned char* data = stbi_load(path.string().c_str(), &_width, &_height, &n, STBI_rgb_alpha);
	if (data != nullptr)
		data_v = std::vector(data, data + _width * _height * 4);

	stbi_image_free(data);
	return data != nullptr;
}
void Texture::initData(const std::vector<uint8_t>& image)
{
	_data = new unsigned char[_width * _height * 4];
	memcpy(_data, image.data(), _width * _height * 4);
}

Material* Material::defaultLit()
{
	static Material instance(Color::white(), true);
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

Material::Material(Color color, bool lit, Texture* texture, float roughness, float reflection, Color emission) : _id(_nextAvailableId++), _lit {lit}, _color {color},
                                                                                                                  _texture {texture}, _emission {emission},
                                                                                                                  _roughness {roughness}, _reflection {reflection}
{
	Scene::materials.push_back(this);

	BufferController::markBufferForUpdate(BufferType::Materials);
}
Material::Material(Color color, bool lit) : Material(color, lit, Texture::defaultTex()) {}
Material::Material(const Material& material) : Material(material._color, material._lit, material._texture, material._roughness, material._reflection, material._emission) {}
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
void Material::setTexture(Texture* texture)
{
	_texture = texture;

	BufferController::markBufferForUpdate(BufferType::Materials);
	BufferController::markBufferForUpdate(BufferType::Textures);
}
void Material::setDiffuseCoef(float diffuseCoef)
{
	_roughness = diffuseCoef;

	BufferController::markBufferForUpdate(BufferType::Materials);
}
void Material::setReflection(float reflection)
{
	_reflection = reflection;

	BufferController::markBufferForUpdate(BufferType::Materials);
}
void Material::setEmission(const Color& emission)
{
	_emission = emission;

	BufferController::markBufferForUpdate(BufferType::Materials);
}
