#include "BufferController.h"

#include "BVH.h"
#include "Graphical.h"
#include "Light.h"
#include "Pathtracer.h"
#include "Scene.h"
#include "Triangle.h"

void BufferController::updateAllBuffers()
{
	updateTexturesBuffer();
	updateMaterialsBuffer();
	updateLightsBuffer();
	updateObjectsBuffer();
	updateTrianglesBuffer();
	updateBVHBuffer();
}

void BufferController::updateTexturesBuffer()
{
	for (const auto& tex : Scene::textures)
		Pathtracer::traceShaderP->fragShader->addTexture2D(tex);

	// Fix this
	glBindTexture(GL_TEXTURE_2D, Pathtracer::traceShaderP->fragShader->textures[0]->id);
}

void BufferController::updateMaterialsBuffer()
{
	std::vector<MaterialStruct> data {};
	for (const auto& mat : Scene::materials)
	{
		MaterialStruct materialStruct {};
		materialStruct.color = mat->color;
		materialStruct.properties1 = glm::vec4(mat->lit, mat->diffuseCoeff, mat->specularCoeff, mat->specularDegree);
		materialStruct.properties2 = glm::vec4(mat->reflection, mat->indexID, 0, 0);

		data.push_back(materialStruct);
	}

	Pathtracer::traceShaderP->fragShader->uboMaterials->setData((float*)data.data(), data.size());
	Pathtracer::traceShaderP->setInt("materialCount", data.size());
}

void BufferController::updateLightsBuffer()
{
	std::vector<LightStruct> data {};
	for (const auto& light : Scene::lights)
	{
		LightStruct lightStruct {};
		lightStruct.pos = glm::vec4(light->getPos(), 0);
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

	Pathtracer::traceShaderP->fragShader->uboLights->setData((float*)data.data(), data.size());
	Pathtracer::traceShaderP->setInt("lightCount", data.size());
}

void BufferController::updateObjectsBuffer()
{
	auto triangleCount = 0;
	std::vector<ObjectStruct> data {};
	for (const auto& obj : Scene::graphicals)
	{
		ObjectStruct objectStruct {};
		objectStruct.data.y = obj->materialNoCopy()->indexID;
		objectStruct.pos = glm::vec4(obj->getPos(), 0);

		if (dynamic_cast<Mesh*>(obj) != nullptr)
		{
			auto mesh = (Mesh*)obj;
			objectStruct.data.x = 0;
			objectStruct.properties.x = triangleCount;
			objectStruct.properties.y = mesh->triangles.size();
			triangleCount += mesh->triangles.size();
		}
		else if (dynamic_cast<Sphere*>(obj) != nullptr)
		{
			auto sphere = (Sphere*)obj;
			objectStruct.data.x = 1;
			objectStruct.properties.x = sphere->radius * sphere->radius;
		}
		else if (dynamic_cast<Plane*>(obj) != nullptr)
		{
			auto plane = (Plane*)obj;
			objectStruct.data.x = 2;
			objectStruct.properties = glm::vec4(plane->normal, 0);
		}

		data.push_back(objectStruct);
	}

	Pathtracer::traceShaderP->fragShader->uboObjects->setData((float*)data.data(), data.size());
	Pathtracer::traceShaderP->setInt("objectCount", data.size());
}

void BufferController::updateTrianglesBuffer()
{
	std::vector<TriangleStruct> data {};
	for (const auto& triangle : Scene::triangles)
	{
		TriangleStruct triangleStruct {};
		for (int k = 0; k < 3; ++k)
		{
			triangleStruct.vertices[k].posU = glm::vec4(triangle->globalVertexPositions[k], triangle->vertices[k].uvPos.x);
			triangleStruct.vertices[k].normalV = glm::vec4(triangle->globalVertexNormals[k], triangle->vertices[k].uvPos.y);
		}
		triangleStruct.materialIndex = glm::vec4(triangle->mesh->materialNoCopy()->indexID, 0, 0, 0);

		triangleStruct.rows[0] = glm::vec4(triangle->row1, triangle->row1Val);
		triangleStruct.rows[1] = glm::vec4(triangle->row2, triangle->row2Val);
		triangleStruct.rows[2] = glm::vec4(triangle->row3, triangle->row3Val);

		data.push_back(triangleStruct);
	}

	Pathtracer::traceShaderP->fragShader->uboTriangles->setData((float*)data.data(), data.size());
	Pathtracer::traceShaderP->setInt("triangleCount", data.size());
}

void BufferController::updateBVHBuffer()
{
	const auto& nodes = BVHBuilder::nodes;
	std::vector<BVHNodeStruct> data {};
	for (const auto& node : nodes)
	{
		BVHNodeStruct bvhNodeStruct {};
		bvhNodeStruct.min = glm::vec4(node->box.min_, node->leafTrianglesStart);
		bvhNodeStruct.max = glm::vec4(node->box.max_, node->leafTriangleCount);
		bvhNodeStruct.values = glm::vec4(node->hitNext, node->missNext, node->isLeaf, 0);

		data.push_back(bvhNodeStruct);
	}

	Pathtracer::traceShaderP->fragShader->uboBVHNodes->setData((float*)data.data(), data.size());
	Pathtracer::traceShaderP->setInt("bvhNodeCount", data.size());
}
