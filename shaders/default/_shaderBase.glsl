#define FLT_MAX  1000000
#define PI 3.14159265359
#define PHI 1.61803398874

#define RAY_DEFAULT_ARGS FLT_MAX, FLT_MAX, -1, vec3(0), vec3(0), vec2(0)
#define RAY_DEFAULT_ARGS_WO_DIST FLT_MAX, -1, vec3(0), vec3(0), vec2(0)

#define RAY_DEFAULT_ARGS FLT_MAX, FLT_MAX, -1, vec3(0), vec3(0), vec2(0)
#define RAY_DEFAULT_ARGS_WO_DIST FLT_MAX, -1, vec3(0), vec3(0), vec2(0)

struct TexInfo
{
    vec2 size;
};

struct Light
{
    vec3 pos;
    int lightType; // 0 - global, 1 - point
    vec4 color;
    vec4 properties1; // intensity, [PointLight(distance) : GlobalLight(dirX, dirY, dirZ) : AreaLight(size)]
};

struct Material
{
    vec4 color;
    bool lit;
    float diffuseCoeff;
    float reflection;
    float indexID;
    vec3 _pad;
    int texArrayLayerIndex;
};

struct Object
{
    int objType;
    int materialIndex;
    vec2 _pad;
    vec4 pos;
    mat4x4 transform;
    vec4 properties; // [Mesh(trianglesStart, triangleCount) : Sphere(radiusSquared) : Plane(normal)]
};

struct Vertex
{
    vec4 posU;
    vec4 normalV;
};

struct Triangle
{
    Vertex vertices[3];
    vec4 materialIndex; // materialIndex, meshIndex, triIndex
    vec4 rows[3];
};

struct BVHNode
{
    vec4 min;
    vec4 max;
    vec3 values; // hitNext, missNext, isLeaf
};

struct Ray
{
    vec3 pos, dir;
    float maxDis;
    float t;
    int materialIndex;
    vec3 surfaceNormal;
    vec3 interPoint;
    vec2 uvPos;
};

// ----------- BUFFERS -----------
uniform mat4x4 cameraRotMat = mat4x4(1.0);

uniform vec2 texArrayBounds;
layout(binding = 0) uniform sampler2DArray texArray;

layout(std140, binding = 1) uniform TexInfos
{
    TexInfo texInfos[1];
};

layout(std140, binding = 2) uniform Materials
{
    Material materials[1];
};

layout(std140, binding = 3) uniform Lights
{
    Light lights[1];
};

layout(std140, binding = 4) uniform Objects
{
    Object objects[1];
};

layout(std140, binding = 5) /*buffer*/ uniform Triangles
{
    Triangle triangles[];
};

layout(std140, binding = 6) /*buffer*/ uniform BVHNodes
{
    BVHNode nodes[];
};

layout(std430, binding = 7) /*buffer*/ uniform BVHTriIndices
{
    float triIndices[];
};

Material getMaterial(int index)
{
    for (int i = 0; i < materials.length(); i++)
    {
        if (materials[i].indexID == index)
            return materials[i];
    }
    return materials[0];
}

vec3 localToGlobal(vec3 pos, Object obj)
{
    return (obj.transform * vec4(pos, 1.0f)).xyz;
}
vec3 localToGlobalDir(vec3 normal, Object obj)
{
    return normalize((obj.transform * vec4(normal, 0.0f)).xyz);
}

vec3 calcTriangleCenter(Triangle tri)
{
    vec3 p0 = localToGlobal(tri.vertices[0].posU.xyz, objects[int(tri.materialIndex.y)]);
    vec3 p1 = localToGlobal(tri.vertices[1].posU.xyz, objects[int(tri.materialIndex.y)]);
    vec3 p2 = localToGlobal(tri.vertices[2].posU.xyz, objects[int(tri.materialIndex.y)]);
    return (p0 + p1 + p2) * 0.33333333f;
}

/// #include "default/utils.glsl"
/// #include "intersection.glsl"
/// #include "light.glsl"
