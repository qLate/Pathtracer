#pragma once

#include "Utils.h"
#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "GLObject.h"
#include "ShaderProgram.h"

enum class BufferType
{
	None,
	TexInfos = 1,
	Materials = 2,
	Lights = 4,
	Objects = 8,
	Triangles = 16,
	BVHNodes = 32,
};

class BufferController
{
	static constexpr int TEX_INFOS_ALIGN = 4;
	static constexpr int LIGHT_ALIGN = 12;
	static constexpr int MATERIAL_ALIGN = 12;
	static constexpr int OBJECT_ALIGN = 28;
	static constexpr int TRIANGLE_ALIGN = 40;
	static constexpr int BVH_NODE_ALIGN = 16;
	static constexpr int BVH_TRI_INDICES_ALIGN = 1;

	static constexpr glm::ivec3 TEX_ARRAY_BOUNDS = {4096, 4096, 12};

	inline static UPtr<UBO> _uboTexInfos;
	inline static UPtr<UBO> _uboMaterials;
	inline static UPtr<UBO> _uboLights;
	inline static UPtr<UBO> _uboObjects;
	inline static UPtr<SSBO> _ssboTriangles;
	inline static UPtr<SSBO> _ssboBVHNodes;
	inline static UPtr<SSBO> _ssboBVHTriIndices;
	inline static UPtr<GLTexture2DArray> _texArray;

	inline static UPtr<ComputeShaderProgram> _precomputeTriCoefsProgram;

	inline static BufferType _buffersForUpdate;

	static void init();

	static void checkIfBufferUpdateRequired();

public:
	static void initBuffers();

	static void markBufferForUpdate(BufferType bufferType);

	static UPtr<UBO>& uboTexInfos() { return _uboTexInfos; }
	static UPtr<UBO>& uboMaterials() { return _uboMaterials; }
	static UPtr<UBO>& uboLights() { return _uboLights; }
	static UPtr<UBO>& uboObjects() { return _uboObjects; }
	static UPtr<SSBO>& ssboTriangles() { return _ssboTriangles; }
	static UPtr<SSBO>& ssboBVHNodes() { return _ssboBVHNodes; }
	static UPtr<SSBO>& ssboBVHTriIndices() { return _ssboBVHTriIndices; }
	static UPtr<GLTexture2DArray>& texArray() { return _texArray; }

	static void updateTexInfos();
	static void updateMaterials();
	static void updateLights();
	static void updateObjects();
	static void updateTriangles();
	static void updateBVHNodes();

	static void precomputeTriCoefs();

	friend class TraceShader;
	friend class Program;

private:
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
		float id;
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

inline BufferType operator|(BufferType a, BufferType b)
{
	return (BufferType)((int)a | (int)b);
}
inline void operator|=(BufferType& a, BufferType b)
{
	a = a | b;
}
