#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <stb_image.h>

#include "Material.h"

#include "Renderer.h"
#include "Scene.h"
#include "Utils.h"

Texture::Texture(const std::filesystem::path& path)
{
	Scene::textures.push_back(this);

	std::vector<uint8_t> image;
	if (!readImage(image, path)) std::cerr << "Error loading texture: " << path << "\n";
	copyImageData(image);

	texArrayLayerIndex = Renderer::texArray->addTexture(this);
}
bool Texture::readImage(std::vector<uint8_t>& data_v, const std::filesystem::path& path)
{
	int n;
	unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &n, 4);
	if (data != nullptr)
		data_v = std::vector(data, data + width * height * 4);

	stbi_image_free(data);
	return data != nullptr;
}
void Texture::copyImageData(const std::vector<uint8_t>& image)
{
	data = new unsigned char[width * height * 4];
	memcpy(data, image.data(), width * height * 4);
}


Material::Material(Color color, bool lit, Texture* texture, float diffuseCoef, float reflection): lit {lit}, color {color}, texture {texture}, diffuseCoef {diffuseCoef},
                                                                                                   reflection {reflection}
{
	this->id = nextAvailableId++;
	Scene::materials.push_back(this);
}
Material::Material(Color color, bool lit) : Material(color, lit, Texture::defaultTex) {}
Material::Material(const Material& material) : Material(material.color, material.lit, material.texture, material.diffuseCoef, material.reflection) {}
Material::~Material()
{
	std::erase(Scene::materials, this);
}
