#pragma once

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

class BufferController
{
	struct LightStruct
	{
		float lightType;
		glm::vec3 _pad;
		glm::vec4 pos;
		glm::vec4 color;
		glm::vec4 properties1;
	};

	struct MaterialStruct
	{
		glm::vec4 color;
		glm::vec4 properties1;
		glm::vec4 properties2;
	};

	struct ObjectStruct
	{
		glm::vec4 data;
		glm::vec4 pos;
		glm::vec4 properties;
	};

	struct TriangleStruct
	{
		struct VertexStruct
		{
			glm::vec4 posU;
			glm::vec4 normalV;
		};

		VertexStruct vertices[3];
		glm::vec4 materialIndex;
		glm::vec4 rows[3];
	};

	struct BVHNodeStruct
	{
		glm::vec4 min;
		glm::vec4 max;
		glm::vec4 values;
	};

public:
	static void updateAllBuffers();

	static void updateMaterialsBuffer();
	static void updateLightsBuffer();
	static void updateObjectsBuffer();
	static void updateTrianglesBuffer();
	static void updateBVHBuffer();
};
