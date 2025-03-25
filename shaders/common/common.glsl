#extension GL_ARB_bindless_texture : enable
#extension GL_ARB_shading_language_include : enable

#include "utils.glsl"

#define RAY_DEFAULT_ARGS FLT_MAX, FLT_MAX, -1, vec3(0), vec3(0), vec2(0)
#define RAY_DEFAULT_ARGS_WO_DIST FLT_MAX, -1, vec3(0), vec3(0), vec2(0)

#define RAY_DEFAULT_ARGS FLT_MAX, FLT_MAX, -1, vec3(0), vec3(0), vec2(0)
#define RAY_DEFAULT_ARGS_WO_DIST FLT_MAX, -1, vec3(0), vec3(0), vec2(0)

vec3 COLOR_DEBUG = vec3(0);

struct Light
{
    vec3 pos;
    int lightType; // 0 - global, 1 - point
    vec3 color;
    vec4 properties1; // intensity, [PointLight(distance) : GlobalLight(dirX, dirY, dirZ) : AreaLight(size)]
};

struct Material
{
    vec3 color;
    float _pad1;
    bool lit;
    float diffuseCoeff;
    float reflection;
    float id;
    vec3 _pad2;
    int textureIndex;
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
    vec4 info; // materialIndex, meshIndex, baseTriIndex
};

struct BVHNode
{
    vec4 min; // min, trianglesStart
    vec4 max; // max, triangleCount
    ivec4 values; // left, right, isLeaf, parent
    ivec4 links; // hit, miss, boxCalculated
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

layout(std140, binding = 1) uniform Textures
{
    sampler2D textures[1];
};

uniform int materialCount;
layout(std140, binding = 2) uniform Materials
{
    Material materials[1];
};

uniform int lightCount;
layout(std140, binding = 3) uniform Lights
{
    Light lights[1];
};

uniform int objectCount;
layout(std140, binding = 4) uniform Objects
{
    Object objects[1];
};

layout(std140, binding = 5) /*buffer*/ uniform Triangles
{
    Triangle triangles[];
};

Material getMaterial(int id)
{
    for (int i = 0; i < materialCount; i++)
    {
        if (materials[i].id == id)
            return materials[i];
    }
    return materials[0];
}

vec3 localToGlobal(vec3 pos, Object obj)
{
    return (obj.transform * vec4(pos, 1.0f)).xyz;
}
vec3 localToGlobalDir(vec3 dir, Object obj)
{
    return normalize((obj.transform * vec4(dir, 0.0f)).xyz);
}

vec3 getTransformScale(mat4x4 mat) {
    vec3 scale;
    for (int i = 0; i < 3; i++)
        scale[i] = sqrt(mat[i][0] * mat[i][0] + mat[i][1] * mat[i][1] + mat[i][2] * mat[i][2]);
    return scale;
}

mat4x4 getTransformRotation(mat4x4 mat) {
    mat4x4 rotMat = mat4x4(1.0);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            rotMat[i][j] = mat[i][j];
    return rotMat;
}

vec3 getTriangleCenter(Triangle tri)
{
    vec3 p0 = localToGlobal(tri.vertices[0].posU.xyz, objects[int(tri.info.y)]);
    vec3 p1 = localToGlobal(tri.vertices[1].posU.xyz, objects[int(tri.info.y)]);
    vec3 p2 = localToGlobal(tri.vertices[2].posU.xyz, objects[int(tri.info.y)]);
    return (p0 + p1 + p2) * 0.33333333f;
}

void calcTriangleBox(Triangle tri, out vec3 minBound, out vec3 maxBound) {
    vec3 p0 = localToGlobal(tri.vertices[0].posU.xyz, objects[int(tri.info.y)]);
    vec3 p1 = localToGlobal(tri.vertices[1].posU.xyz, objects[int(tri.info.y)]);
    vec3 p2 = localToGlobal(tri.vertices[2].posU.xyz, objects[int(tri.info.y)]);

    minBound = min(min(p0, p1), p2) - vec3(0.0001);
    maxBound = max(max(p0, p1), p2) + vec3(0.0001);
}
