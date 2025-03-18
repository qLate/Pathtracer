#include "BufferController.h"

#include "BVH.h"
#include "Graphical.h"
#include "Light.h"
#include "Renderer.h"
#include "Scene.h"
#include "Triangle.h"

void BufferController::init()
{
	precomputeTriCoefsProgram = new ComputeShaderProgram("shaders/compute/precomputeTriCoefs.comp");
}
void BufferController::uninit()
{
	delete precomputeTriCoefsProgram;
}

void BufferController::recalculateTriangleCoefs()
{
	precomputeTriCoefsProgram->use();

	int triCount = Scene::triangles.size();
	ComputeShaderProgram::dispatch({triCount / 64 + 1, 1, 1});
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void BufferController::updateBuffers()
{
	bindBuffers();

	updateTexInfosBuffer();
	updateMaterialsBuffer();
	updateLightsBuffer();
	updateTrianglesBuffer();
	updateObjectsBuffer();
	updateBVHNodesBuffer();
}
void BufferController::bindBuffers()
{
	Renderer::renderProgram->fragShader->uboTexInfos->bindDefault();
	Renderer::renderProgram->fragShader->uboMaterials->bindDefault();
	Renderer::renderProgram->fragShader->uboLights->bindDefault();
	Renderer::renderProgram->fragShader->uboObjects->bindDefault();
	Renderer::renderProgram->fragShader->ssboTriangles->bindDefault();
	Renderer::renderProgram->fragShader->ssboBVHNodes->bindDefault();
	Renderer::renderProgram->fragShader->ssboBVHTriIndices->bindDefault();
}

void BufferController::updateTexInfosBuffer()
{
	auto textures = Scene::textures;
	std::vector<TexInfoStruct> data(textures.size());
#pragma omp parallel for
	for (int i = 0; i < textures.size(); i++)
	{
		auto tex = textures[i];

		TexInfoStruct texInfoStruct;
		texInfoStruct.sizes = glm::vec4(tex->width, tex->height, 0, 0);
		data[i] = texInfoStruct;
	}
	Renderer::renderProgram->fragShader->uboTexInfos->setData((float*)data.data(), data.size());
}

void BufferController::updateMaterialsBuffer()
{
	auto materials = Scene::materials;
	std::vector<MaterialStruct> data(materials.size());
#pragma omp parallel for
	for (int i = 0; i < materials.size(); i++)
	{
		auto mat = materials[i];

		MaterialStruct materialStruct {};
		materialStruct.color = mat->color;
		materialStruct.lit = mat->lit;
		materialStruct.diffuseCoeff = mat->diffuseCoef;
		materialStruct.reflection = mat->reflection;
		materialStruct.indexID = mat->id;
		materialStruct.texArrayLayerIndex = mat->texture->texArrayLayerIndex;

		data[i] = materialStruct;
	}
	Renderer::renderProgram->fragShader->uboMaterials->setData((float*)data.data(), data.size());
}

void BufferController::updateLightsBuffer()
{
	auto lights = Scene::lights;
	std::vector<LightStruct> data(lights.size());
#pragma omp parallel for
	for (int i = 0; i < lights.size(); i++)
	{
		auto light = lights[i];

		LightStruct lightStruct {};
		lightStruct.pos = light->getPos();
		lightStruct.color = light->color;
		lightStruct.properties1.x = light->intensity;

		if (dynamic_cast<DirectionalLight*>(light) != nullptr)
		{
			auto globalLight = (DirectionalLight*)light;
			lightStruct.lightType = 0;
			lightStruct.properties1 = glm::vec4(lightStruct.properties1.x, globalLight->dir);
		}
		else if (dynamic_cast<PointLight*>(light))
		{
			auto pointLight = (PointLight*)light;
			lightStruct.lightType = 1;
			lightStruct.properties1.y = pointLight->dis;
		}

		data[i] = lightStruct;
	}
	Renderer::renderProgram->fragShader->uboLights->setData((float*)data.data(), data.size());
}

void BufferController::updateObjectsBuffer()
{
	auto triangleCount = 0;
	auto graphicals = Scene::graphicals;
	std::vector<ObjectStruct> data(graphicals.size());
#pragma omp parallel for
	for (int i = 0; i < graphicals.size(); i++)
	{
		auto obj = graphicals[i];

		ObjectStruct objectStruct {};
		objectStruct.materialIndex = obj->materialNoCopy()->id;
		objectStruct.pos = glm::vec4(obj->getPos(), 0);
		objectStruct.transform = translate(glm::mat4(1), obj->getPos()) * mat4_cast(obj->getRot()) * scale(glm::mat4(1), obj->getScale());

		if (dynamic_cast<Mesh*>(obj) != nullptr)
		{
			auto mesh = (Mesh*)obj;
			objectStruct.objType = 0;
			objectStruct.properties.x = triangleCount;
			objectStruct.properties.y = mesh->triangles.size();
			triangleCount += mesh->triangles.size();
		}
		else if (dynamic_cast<Sphere*>(obj) != nullptr)
		{
			auto sphere = (Sphere*)obj;
			objectStruct.objType = 1;
			objectStruct.properties.x = sphere->radius * sphere->radius;
		}
		else if (dynamic_cast<Plane*>(obj) != nullptr)
		{
			auto plane = (Plane*)obj;
			objectStruct.objType = 2;
			objectStruct.properties = glm::vec4(plane->normal, 0);
		}

		data[i] = objectStruct;
	}
	Renderer::renderProgram->fragShader->uboObjects->setData((float*)data.data(), data.size());

	recalculateTriangleCoefs();
}

void BufferController::updateTrianglesBuffer()
{
	auto triangles = Scene::triangles;
	std::vector<TriangleStruct> data(triangles.size());
#pragma omp parallel for
	for (int i = 0; i < triangles.size(); i++)
	{
		auto triangle = triangles[i];
		TriangleStruct triangleStruct {};
		for (int k = 0; k < 3; ++k)
		{
			triangleStruct.vertices[k].posU = glm::vec4(triangle->vertices[k].pos, triangle->vertices[k].uvPos.x);
			triangleStruct.vertices[k].normalV = glm::vec4(triangle->vertices[k].normal, triangle->vertices[k].uvPos.y);
		}
		triangleStruct.materialIndex = glm::vec4(triangle->mesh->materialNoCopy()->id, triangle->mesh->indexID, 0, 0);

		data[i] = triangleStruct;
	}
	Renderer::renderProgram->fragShader->ssboTriangles->setData((float*)data.data(), data.size());
}

void BufferController::updateBVHNodesBuffer()
{
	auto nodes = BVH::nodes;
	std::vector<BVHNodeStruct> data(nodes.size());
#pragma omp parallel for
	for (int i = 0; i < nodes.size(); i++)
	{
		const auto& node = nodes[i];
		BVHNodeStruct bvhNodeStruct;
		bvhNodeStruct.min = glm::vec4(node->box.min_, node->leafTrianglesStart);
		bvhNodeStruct.max = glm::vec4(node->box.max_, node->leafTriangleCount);
		bvhNodeStruct.values = glm::vec4(node->hitNext, node->missNext, node->isLeaf, 0);

		data[i] = bvhNodeStruct;
	}
	Renderer::renderProgram->fragShader->ssboBVHNodes->setData((float*)data.data(), data.size());

	updateBVHTriangleIndices();
}
void BufferController::updateBVHTriangleIndices()
{
	auto indices = BVH::originalTriIndices;
	std::vector<uint32_t> data(indices.size());
#pragma omp parallel for
	for (int i = 0; i < indices.size(); i++)
		data[i] = indices[i];
	Renderer::renderProgram->fragShader->ssboBVHTriIndices->setData((float*)data.data(), data.size());
}
