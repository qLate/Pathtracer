#include "BufferController.h"

#include "BVH.h"
#include "Graphical.h"
#include "Light.h"
#include "Model.h"
#include "MyMath.h"
#include "Renderer.h"
#include "Scene.h"
#include "Triangle.h"

void BufferController::init()
{
	_uboTextures = make_unique<UBO>(TEXTURE_ALIGN, 1);
	_uboMaterials = make_unique<SSBO>(MATERIAL_ALIGN, 2);
	_uboLights = make_unique<UBO>(LIGHT_ALIGN, 3);
	_ssboObjects = make_unique<SSBO>(OBJECT_ALIGN, 4);
	_ssboTriangles = make_unique<SSBO>(TRIANGLE_ALIGN, 5);
	_ssboBVHNodes = make_unique<SSBO>(BVH_NODE_ALIGN, 6);
	_ssboPrimObjIndices = make_unique<SSBO>(PRIM_OBJ_INDICES_ALIGN, 7);

	_uboTextures->setStorage(1000, GL_DYNAMIC_STORAGE_BIT);
	_uboLights->setStorage(1000, GL_DYNAMIC_STORAGE_BIT);
}

void BufferController::checkIfBufferUpdateRequired()
{
	if (Utils::hasFlag(_buffersForUpdate, BufferType::Textures))
		updateTexInfos();
	if (Utils::hasFlag(_buffersForUpdate, BufferType::Materials))
		updateMaterials();
	if (Utils::hasFlag(_buffersForUpdate, BufferType::Lights))
		updateLights();
	if (Utils::hasFlag(_buffersForUpdate, BufferType::Triangles))
	{
		updateTriangles();
		BVH::rebuildBVH();
	}
	if (Utils::hasFlag(_buffersForUpdate, BufferType::Objects))
		updateObjects();

	_buffersForUpdate = BufferType::None;
}

void BufferController::markBufferForUpdate(BufferType bufferType)
{
	_buffersForUpdate |= bufferType;
}

void BufferController::initBuffers()
{
	bindBuffers();

	updateTexInfos();
	updateMaterials();
	updateLights();
	updateTriangles();
	updateObjects();

	_buffersForUpdate = BufferType::None;
}
void BufferController::bindBuffers()
{
	_uboTextures->bindDefault();
	_uboMaterials->bindDefault();
	_uboLights->bindDefault();
	_ssboObjects->bindDefault();
	_ssboTriangles->bindDefault();
	_ssboBVHNodes->bindDefault();
	_ssboPrimObjIndices->bindDefault();
}

void BufferController::updateTexInfos()
{
	auto textures = Scene::textures;
	std::vector<TextureStruct> data(textures.size());
	//#pragma omp parallel for
	for (int i = 0; i < textures.size(); i++)
	{
		auto tex = textures[i];

		TextureStruct texInfoStruct{};
		texInfoStruct.handle = tex->glTex()->getHandle();

		data[i] = texInfoStruct;
	}
	_uboTextures->setSubData((float*)data.data(), data.size());
	Renderer::resetSamples();
}

void BufferController::updateMaterials()
{
	auto materials = Scene::materials;
	std::ranges::sort(materials, [](const Material* a, const Material* b) { return a->id() < b->id(); });
	std::vector<MaterialStruct> data(materials.size());
	#pragma omp parallel for
	for (int i = 0; i < materials.size(); i++)
	{
		auto mat = materials[i];

		MaterialStruct materialStruct{};
		materialStruct.color = mat->color().xyz;
		materialStruct.id = mat->id();
		materialStruct.lit = mat->lit();
		materialStruct.roughness = mat->roughness();
		materialStruct.textureIndex = mat->texture()->id();
		materialStruct.emission = mat->emission().xyz;
		materialStruct.metallic = mat->metallic();

		data[i] = materialStruct;
	}
	_uboMaterials->ensureDataCapacity(data.size());
	_uboMaterials->setSubData((float*)data.data(), data.size());
	Renderer::renderProgram()->fragShader()->setInt("materialCount", materials.size());
	Renderer::resetSamples();
}

void BufferController::updateLights()
{
	auto lights = Scene::lights;
	std::vector<LightStruct> data(lights.size());
	#pragma omp parallel for
	for (int i = 0; i < lights.size(); i++)
	{
		auto light = lights[i];

		LightStruct lightStruct{};
		lightStruct.pos = light->pos();
		lightStruct.color = light->color();
		lightStruct.properties1.x = light->intensity();

		if (dynamic_cast<DirectionalLight*>(light) != nullptr)
		{
			auto globalLight = (DirectionalLight*)light;
			lightStruct.lightType = 0;
			lightStruct.properties1.yzw = -(globalLight->rot() * vec3::FORWARD);
		}
		else if (dynamic_cast<PointLight*>(light))
		{
			auto pointLight = (PointLight*)light;
			lightStruct.lightType = 1;
			lightStruct.properties1.y = pointLight->dis();
		}

		data[i] = lightStruct;
	}

	auto graphicals = Scene::graphicals;
	for (int i = 0; i < graphicals.size(); i++)
	{
		if (graphicals[i]->materialNoCopy()->emission() == Color::clear()) continue;

		if (auto mesh = dynamic_cast<Mesh*>(graphicals[i]))
		{
			const auto& triangles = mesh->model()->baseTriangles();
			if (triangles.empty()) continue;

			int triStartIndex = mesh->model()->triStartIndex();
			for (int j = 0; j < triangles.size(); j++)
			{
				auto tri = triangles[j];
				auto v0 = mesh->localToGlobalPos(tri->vertices()[0].pos);
				auto v1 = mesh->localToGlobalPos(tri->vertices()[1].pos);
				auto v2 = mesh->localToGlobalPos(tri->vertices()[2].pos);
				auto triArea = 0.5f * length(cross(v1 - v0, v2 - v0));

				LightStruct lightStruct{};
				lightStruct.lightType = 2;
				lightStruct.properties1.xyz = {triStartIndex + j, triArea, i};

				data.push_back(lightStruct);
			}
		}
	}

	_uboLights->setSubData((float*)data.data(), data.size());
	Renderer::renderProgram()->fragShader()->setInt("lightCount", data.size());
	Renderer::resetSamples();
}

void BufferController::updateObjects()
{
	auto graphicals = Scene::graphicals;
	std::vector<ObjectStruct> data(graphicals.size());
	std::vector<float> primIndicesData;
	std::mutex mutex;
	#pragma omp parallel for
	for (int i = 0; i < graphicals.size(); i++)
	{
		if (graphicals[i] == nullptr) continue;
		auto obj = graphicals[i];

		ObjectStruct objectStruct{};
		objectStruct.materialId = obj->materialNoCopy()->id();
		objectStruct.pos = {obj->pos(), 0};
		objectStruct.transform = obj->getTransform();

		bool isPrim = true;
		if (dynamic_cast<Mesh*>(obj) != nullptr)
		{
			auto mesh = (Mesh*)obj;
			objectStruct.objType = 0;
			if (mesh->model() != nullptr)
				objectStruct.properties = {mesh->model()->triStartIndex(), mesh->model()->baseTriangles().size(), mesh->model()->bvhRootNode(), 0};
			else
				objectStruct.properties.xyz = {-1, -1, -1};
			isPrim = false;
		}
		else if (dynamic_cast<Sphere*>(obj) != nullptr)
		{
			auto sphere = (Sphere*)obj;
			objectStruct.objType = 1;
			objectStruct.properties.x = sphere->radius() * sphere->radius();
		}
		else if (dynamic_cast<Plane*>(obj) != nullptr)
		{
			auto plane = (Plane*)obj;
			objectStruct.objType = 2;
			objectStruct.properties.xyz = vec3::UP;
		}

		data[i] = objectStruct;

		if (isPrim)
		{
			mutex.lock();
			primIndicesData.push_back(i);
			mutex.unlock();
		}
	}
	_ssboObjects->ensureDataCapacity(data.size());
	_ssboObjects->setSubData((float*)data.data(), data.size());
	Renderer::renderProgram()->fragShader()->setInt("objectCount", graphicals.size());

	_lastPrimObjCount = primIndicesData.size();
	_ssboPrimObjIndices->ensureDataCapacity(primIndicesData.size());
	_ssboPrimObjIndices->setSubData(primIndicesData.data(), primIndicesData.size());
	Renderer::renderProgram()->fragShader()->setInt("primObjCount", primIndicesData.size());

	Renderer::resetSamples();
}

void BufferController::updateTriangles()
{
	auto triangles = Scene::baseTriangles;
	std::vector<TriangleStruct> data(triangles.size());
	#pragma omp parallel for
	for (int i = 0; i < triangles.size(); i++)
	{
		auto triangle = triangles[i];
		TriangleStruct triangleStruct{};
		for (int k = 0; k < 3; ++k)
		{
			triangleStruct.vertices[k].posU = glm::vec4(triangle->vertices()[k].pos, triangle->vertices()[k].uvPos.x);
			triangleStruct.vertices[k].normalV = glm::vec4(triangle->vertices()[k].normal, triangle->vertices()[k].uvPos.y);
		}
		//triangleStruct.info = glm::vec4(triangle->mesh()->materialNoCopy()->id(), triangle->mesh()->indexId(), 0, 0);

		data[i] = triangleStruct;
	}
	_ssboTriangles->ensureDataCapacity(data.size());
	_ssboTriangles->setSubData((float*)data.data(), data.size());
	Renderer::renderProgram()->fragShader()->setInt("triCount", triangles.size());

	Renderer::resetSamples();
}
