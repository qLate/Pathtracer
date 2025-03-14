#pragma once

#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

class SSBO;
class ComputeShaderProgram;

class BufferController
{
	static constexpr int TEX_INFOS_ALIGN = 4;
	static constexpr int LIGHT_ALIGN = 12;
	static constexpr int MATERIAL_ALIGN = 12;
	static constexpr int OBJECT_ALIGN = 28;
	static constexpr int TRIANGLE_ALIGN = 40;
	static constexpr int BVH_NODE_ALIGN = 12;
	static constexpr int BVH_TRI_INDICES_ALIGN = 1;

	struct TexInfoStruct
	{
		glm::vec4 sizes;
	};

	struct MaterialStruct
	{
		glm::vec4 color;
		bool lit;
		float diffuseCoeff;
		float reflection;
		float indexID;
		glm::vec3 _pad;
		int texArrayLayerIndex;
	};

	struct LightStruct
	{
		glm::vec3 pos;
		int lightType;
		glm::vec4 color;
		glm::vec4 properties1;
	};

	struct ObjectStruct
	{
		int objType;
		int materialIndex;
		glm::vec2 _pad;
		glm::vec4 pos;
		glm::mat4x4 transform;
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

	//struct BVHLinkStruct
	//{
	//	glm::vec2 _pad;
	//	int hit;
	//	int miss;
	//};

	inline static ComputeShaderProgram* precomputeTriCoefsProgram;

	static void recalculateTriangleCoefs();

public:
	static void init();
	static void uninit();

	static void updateBuffers();

	static void updateTexInfosBuffer();
	static void updateMaterialsBuffer();
	static void updateLightsBuffer();
	static void updateObjectsBuffer();
	static void updateTrianglesBuffer();
	static void updateBVHNodesBuffer();
	static void updateBVHTriangleIndices();

	friend class TraceShader;
};
