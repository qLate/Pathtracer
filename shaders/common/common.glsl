#extension GL_ARB_bindless_texture : enable
#extension GL_ARB_shading_language_include : enable

#include "utils.glsl"

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_TRIANGLE 2

#define OBJ_TYPE_MESH 0
#define OBJ_TYPE_SPHERE 1
#define OBJ_TYPE_PLANE 2

#define RAY_DEFAULT_ARGS FLT_MAX, vec3(0), vec3(0), vec2(0), -1, -1
#define RAY_DEFAULT_ARGS_WO_DIST vec3(0), vec3(0), vec2(0), -1, -1

#define UP vec3(0, 1, 0)
#define DOWN vec3(0, -1, 0)
#define LEFT vec3(-1, 0, 0)
#define RIGHT vec3(1, 0, 0)
#define FORWARD vec3(0, 0, 1)
#define BACK vec3(0, 0, -1)
#define ZERO vec3(0, 0, 0)
#define ONE vec3(1, 1, 1)

#define WHITE vec3(1, 1, 1)
#define BLACK vec3(0, 0, 0)
#define RED vec3(1, 0, 0)
#define GREEN vec3(0, 1, 0)
#define BLUE vec3(0, 0, 1)
#define YELLOW vec3(1, 1, 0)
#define CYAN vec3(0, 1, 1)
#define MAGENTA vec3(1, 0, 1)
#define GRAY vec3(0.5, 0.5, 0.5)
#define DARK_GRAY vec3(0.2, 0.2, 0.2)
#define PURPLE vec3(0.5, 0, 0.5)

vec3 COLOR_DEBUG = vec3(-1);

void red() {
    COLOR_DEBUG = RED;
}
void green() {
    COLOR_DEBUG = GREEN;
}
void blue() {
    COLOR_DEBUG = BLUE;
}

struct Light
{
    vec3 pos;
    int lightType; // 0 - global, 1 - point
    vec3 color;
    vec4 properties1; // intensity, [PointLight(distance), GlobalLight(dirX, dirY, dirZ), TriangleLight(triIndex, area)]
};

struct Material
{
    vec3 color;
    int id;
    bool lit;
    float roughness;
    float metallic;
    int textureIndex;
    vec3 emission;
    float _pad;
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
    vec4 info; // materialIndex, meshIndex, interpolateNormals
};

struct BVHNode
{
    vec4 min; // min, triIndex
    vec4 max; // max
    ivec4 values; // left, right, isLeaf, parent
    ivec4 links; // hit, miss, boxCalculated
};

struct Ray
{
    vec3 pos, dir;
    float t;
    vec3 surfaceNormal;
    vec3 hitPoint;
    vec2 uv;
    int hitObjIndex, hitTriIndex;
};

// ----------- BUFFERS -----------
uniform mat4x4 cameraRotMat = mat4x4(1.0);

layout(std140, binding = 1) uniform Textures
{
    sampler2D textures[1];
};

uniform int materialCount;
layout(std140, binding = 2) /*buffer*/ uniform Materials
{
    Material materials[];
};

uniform int lightCount;
layout(std140, binding = 3) uniform Lights
{
    Light lights[1];
};

uniform int objectCount;
layout(std140, binding = 4) /*buffer*/ uniform Objects
{
    Object objects[];
};

uniform int triCount;
layout(std140, binding = 5) /*buffer*/ uniform Triangles
{
    Triangle triangles[];
};

Material findMaterial(int id)
{
    int left = 0;
    int right = materialCount - 1;

    while (left <= right)
    {
        int split = (left + right) / 2;
        int splitVal = materials[split].id;

        if (splitVal < id)
            left = split + 1;
        else if (splitVal > id)
            right = split - 1;
        else
            return materials[split];
    }
    return materials[0];
}

// Material findMaterial(int id)
// {
//     for (int i = 0; i < materialCount; i++)
//     {
//         if (materials[i].id == id)
//             return materials[i];
//     }
//     return materials[0];
// }

vec3 localToGlobal(vec3 pos, Object obj)
{
    return (obj.transform * vec4(pos, 1.0f)).xyz;
}
vec3 globalToLocal(vec3 pos, Object obj)
{
    return (inverse(obj.transform) * vec4(pos, 1.0f)).xyz;
}

vec3 localToGlobalDir(vec3 dir, Object obj)
{
    return normalize((obj.transform * vec4(dir, 0.0f)).xyz);
}
vec3 globalToLocalDir(vec3 dir, Object obj)
{
    return normalize((inverse(obj.transform) * vec4(dir, 0.0f)).xyz);
}

vec3 getTransformScale(mat4x4 mat)
{
    vec3 scale;
    for (int i = 0; i < 3; i++)
        scale[i] = sqrt(mat[i][0] * mat[i][0] + mat[i][1] * mat[i][1] + mat[i][2] * mat[i][2]);
    return scale;
}

mat4x4 getTransformRotation(mat4x4 mat)
{
    mat4x4 rotMat = mat4x4(1.0);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            rotMat[i][j] = mat[i][j];
    return rotMat;
}

void calcGlobalTriVertices(Triangle tri, Object obj, out vec3 p0, out vec3 p1, out vec3 p2)
{
    p0 = localToGlobal(tri.vertices[0].posU.xyz, obj);
    p1 = localToGlobal(tri.vertices[1].posU.xyz, obj);
    p2 = localToGlobal(tri.vertices[2].posU.xyz, obj);
}

Light findTriLight(int triIndex)
{
    for (int i = 0; i < lightCount; i++)
    {
        if (lights[i].lightType == LIGHT_TYPE_TRIANGLE && int(lights[i].properties1.x) == triIndex)
            return lights[i];
    }
    return lights[0];
}
