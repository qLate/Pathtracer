#include "BufferController.h"

#include "BVH.h"
#include "Graphical.h"
#include "Light.h"
#include "Renderer.h"
#include "Scene.h"
#include "Triangle.h"
#include "Utils.h"


void BufferController::init()
{
	precomputeTriCoefsProgram = new ComputeShaderProgram("shaders/compute/precomputeTriCoefs.comp");

	updateBuffers();
}
void BufferController::uninit()
{
	delete precomputeTriCoefsProgram;
}

void BufferController::recalculateTriangleCoefs()
{
	precomputeTriCoefsProgram->use();

	int size = ceil(sqrt(Scene::triangles.size()));
	precomputeTriCoefsProgram->setInt("clusterSize", size);
	precomputeTriCoefsProgram->setInt("triangleCount", Scene::triangles.size());

	ComputeShaderProgram::dispatch({size, size, 1});
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void BufferController::updateBuffers()
{
	Renderer::renderProgram->use();

	updateTexInfosBuffer();
	updateMaterialsBuffer();
	updateLightsBuffer();
	updateTrianglesBuffer();
	updateObjectsBuffer();
	updateBVHNodesBuffer();
	//updateBVHLinksBuffer();
}

void BufferController::updateTexInfosBuffer()
{
	std::vector<TexInfoStruct> data {};
	for (const auto& tex : Scene::textures)
	{
		TexInfoStruct texInfoStruct {};
		texInfoStruct.sizes = glm::vec4(tex->width, tex->height, 0, 0);
		data.push_back(texInfoStruct);
	}

	Renderer::renderProgram->fragShader->uboTexInfos->setData((float*)data.data(), data.size());
	Renderer::renderProgram->setInt("texInfoCount", data.size());
}

void BufferController::updateMaterialsBuffer()
{
	std::vector<MaterialStruct> data {};
	for (const auto& mat : Scene::materials)
	{
		MaterialStruct materialStruct {};
		materialStruct.color = mat->color;
		materialStruct.lit = mat->lit;
		materialStruct.diffuseCoeff = mat->diffuseCoef;
		materialStruct.reflection = mat->reflection;
		materialStruct.indexID = mat->id;
		materialStruct.texArrayLayerIndex = mat->texture->texArrayLayerIndex;

		data.push_back(materialStruct);
	}

	Renderer::renderProgram->fragShader->uboMaterials->setData((float*)data.data(), data.size());
	Renderer::renderProgram->setInt("materialCount", data.size());
}

void BufferController::updateLightsBuffer()
{
	std::vector<LightStruct> data {};
	for (const auto& light : Scene::lights)
	{
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

		data.push_back(lightStruct);
	}

	Renderer::renderProgram->fragShader->uboLights->setData((float*)data.data(), data.size());
	Renderer::renderProgram->setInt("lightCount", data.size());
}

void BufferController::updateObjectsBuffer()
{
	auto triangleCount = 0;
	std::vector<ObjectStruct> data {};
	for (const auto& obj : Scene::graphicals)
	{
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

		data.push_back(objectStruct);
	}

	Renderer::renderProgram->fragShader->uboObjects->setData((float*)data.data(), data.size());
	Renderer::renderProgram->setInt("objectCount", data.size());

	recalculateTriangleCoefs();
}

void BufferController::updateTrianglesBuffer()
{
	TimeMeasurer tm;
	std::vector<TriangleStruct> data {};
	for (const auto& triangle : Scene::triangles)
	{
		TriangleStruct triangleStruct {};
		for (int k = 0; k < 3; ++k)
		{
			triangleStruct.vertices[k].posU = glm::vec4(triangle->vertices[k].pos, triangle->vertices[k].uvPos.x);
			triangleStruct.vertices[k].normalV = glm::vec4(triangle->vertices[k].normal, triangle->vertices[k].uvPos.y);
		}
		triangleStruct.materialIndex = glm::vec4(triangle->mesh->materialNoCopy()->id, triangle->mesh->indexID, 0, 0);

		data.push_back(triangleStruct);
	}
	Renderer::renderProgram->fragShader->ssboTriangles->setData((float*)data.data(), data.size(), GL_STREAM_DRAW);
	Renderer::renderProgram->setInt("triangleCount", data.size());
}

void BufferController::updateBVHNodesBuffer()
{
	std::vector<BVHNodeStruct> data {};
	for (int i = 0; i < BVHBuilder::nodes.size(); i++)
	{
		const auto& node = BVHBuilder::nodes[i];
		BVHNodeStruct bvhNodeStruct {};
		bvhNodeStruct.min = glm::vec4(node->box.min_, node->leafTrianglesStart);
		bvhNodeStruct.max = glm::vec4(node->box.max_, node->leafTriangleCount);
		bvhNodeStruct.values = glm::vec4(node->hitNext, node->missNext, node->isLeaf, 0);

		data.push_back(bvhNodeStruct);
	}

	Renderer::renderProgram->fragShader->ssboBVHNodes->setData((float*)data.data(), data.size());
	Renderer::renderProgram->setInt("bvhNodeCount", data.size());
}

//void BufferController::updateBVHLinksBuffer()
//{
//	if (BVHBuilder::links6Sided[0].empty()) return;
//	std::vector<BVHLinkStruct> data {};
//	for (int i = 0; i < BVHBuilder::links6Sided.size(); i++)
//	{
//		for (int j = 0; j < BVHBuilder::links6Sided[i].size(); j++)
//		{
//			auto [hit, miss] = BVHBuilder::links6Sided[i][j];
//			data.push_back({{}, hit, miss});
//		}
//	}
//	Renderer::shaderP->fragShader->ssboBVHLinks->setData((float*)data.data(), data.size());
//	Renderer::shaderP->setInt("bvhLinkCount", data.size());
//}
