#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

#include "Material.h"

#include "BufferController.h"
#include "Renderer.h"
#include "Scene.h"
#include "Utils.h"

Texture::Texture(const std::filesystem::path& path)
{
	Scene::textures.push_back(this);

	std::vector<uint8_t> image;
	if (!readImage(image, path)) Debug::logError("Error loading texture: ", path);
	setImageData(image);

	_texArrayLayerIndex = Renderer::texArray()->addTexture(this);

	BufferController::markBufferForUpdate(BufferType::TexInfos);
}

Texture* Texture::defaultTex()
{
	static Texture instance("assets/textures/default.png");
	return &instance;
}

bool Texture::readImage(std::vector<uint8_t>& data_v, const std::filesystem::path& path)
{
	int n;
	unsigned char* data = stbi_load(path.string().c_str(), &_width, &_height, &n, 4);
	if (data != nullptr)
		data_v = std::vector(data, data + _width * _height * 4);

	stbi_image_free(data);
	return data != nullptr;
}
void Texture::setImageData(const std::vector<uint8_t>& image)
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
Material* Material::debugLine()
{
	static Material instance(Color::red(), false);
	return &instance;
}

Material::Material(Color color, bool lit, Texture* texture, float diffuseCoef, float reflection): _lit {lit}, _color {color}, _texture {texture}, _diffuseCoef {diffuseCoef},
                                                                                                  _reflection {reflection}
{
	this->_id = _nextAvailableId++;
	Scene::materials.push_back(this);

	BufferController::markBufferForUpdate(BufferType::Materials);
}
Material::Material(Color color, bool lit) : Material(color, lit, Texture::defaultTex()) {}
Material::Material(const Material& material) : Material(material._color, material._lit, material._texture, material._diffuseCoef, material._reflection) {}
Material::~Material()
{
	std::erase(Scene::materials, this);
}
