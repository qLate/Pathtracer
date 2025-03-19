#pragma once

#include "Utils.h"
#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

class UBO;
class SSBO;
class ComputeShaderProgram;

class BufferController
{
	static constexpr int TEX_INFOS_ALIGN = 4;
	static constexpr int LIGHT_ALIGN = 12;
	static constexpr int MATERIAL_ALIGN = 12;
	static constexpr int OBJECT_ALIGN = 28;
	static constexpr int TRIANGLE_ALIGN = 40;
	static constexpr int BVH_NODE_ALIGN = 16;
	static constexpr int BVH_TRI_INDICES_ALIGN = 1;

	inline static UPtr<ComputeShaderProgram> precomputeTriCoefsProgram;

	static void recalculateTriangleCoefs();

public:
	inline static UPtr<UBO> uboTexInfos;
	inline static UPtr<UBO> uboMaterials;
	inline static UPtr<UBO> uboLights;
	inline static UPtr<UBO> uboObjects;
	inline static UPtr<SSBO> ssboTriangles;
	inline static UPtr<SSBO> ssboBVHNodes;
	inline static UPtr<SSBO> ssboBVHTriIndices;

	static void init();

	static void writeBuffers();
	static void bindBuffers();

	static void writeTexInfos();
	static void writeMaterials();
	static void writeLights();
	static void writeObjects();
	static void writeTriangles();
	static void writeBVHNodes();
	static void writeBVHTriIndices();

	friend class TraceShader;

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
		glm::ivec4 values;
		glm::ivec4 links;
	};
};
