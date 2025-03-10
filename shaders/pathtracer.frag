#version 460 core
out vec4 outColor;

#define FLT_MAX  1000000
#define PI 3.14159265359
#define PHI 1.61803398874

#define RAY_DEFAULT_ARGS FLT_MAX, FLT_MAX, -1, vec3(0), vec3(0), vec2(0)
#define RAY_DEFAULT_ARGS_WO_DIST FLT_MAX, -1, vec3(0), vec3(0), vec2(0)

/// #include "default/utils.glsl"
float random(in vec2 xy, in float seed);
bool solveQuadratic(float a, float b, float c, inout float x0, inout float x1);

// ----------- OPTIONS -----------
//#define SHOW_BOXES

// ----------- SETTINGS -----------
const float boxLineWidth = 0.02;
uniform int maxRayBounce;
uniform int samplesPerPixel = 1;

// ----------- CAMERA -----------
uniform vec2 pixelSize;
uniform vec2 screenSize;
uniform float focalDistance;
uniform float lensRadius;
uniform vec3 cameraPos;
uniform vec4 bgColor = vec4(0, 0, 0, 1);

struct Light
{
    float lightType; // 0 - GlobalLight, 1 - PointLight
    vec4 pos;
    vec4 color;
    vec4 properties1; // intensity, [PointLight(distance) : GlobalLight(dirX, dirY, dirZ) : AreaLight(size)]
};

struct Material
{
    vec4 color;
    vec4 properties1; // lit, diffuse coeff, specular coeff, specular degree
    vec4 properties2; // reflection, indexID
};

struct Object
{
    vec4 data; // objType, materialIndex
    vec4 pos; // pos
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
    vec4 materialIndex; // materialIndex, normal
    vec4 rows[3];
};

struct BVHNode
{
    vec4 min; // min, trianglesStart
    vec4 max; // max, triangleCount
    vec4 values; // hitNext, missNext, isLeaf
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

uniform int textureCount;
uniform sampler2D[] textures;

uniform int materialCount;
layout(std140, binding = 1) uniform Materials
{
    Material materials[1];
};

uniform int lightCount;
layout(std140, binding = 2) uniform Lights
{
    Light lights[1];
};

uniform int objectCount;
layout(std140, binding = 3) uniform Objects
{
    Object objects[1];
};

uniform int triangleCount;
layout(std140, binding = 4) uniform Triangles
{
    Triangle triangles[1];
};

uniform int bvhNodeCount;
layout(std140, binding = 5) uniform BVHNodes
{
    BVHNode nodes[1];
};

Material getMaterial(int index)
{
    for (int i = 0; i < materialCount; i++)
    {
        if (materials[i].properties2.y == index)
            return materials[i];
    }
    return materials[0];
}


// **************************************************************************
// ------------------------------ INTERSECTION ------------------------------
// **************************************************************************
/// #include "intersection.glsl"
vec3 getTriangleNormalAt(Triangle tri, float u, float v, bool invert);
bool intersectTriangle(inout Ray ray, Triangle tri);
bool intersectSphere(inout Ray ray, Object sphere);
bool intersectPlane(inout Ray ray, Object plane);
bool intersectAABBForGizmo(inout Ray ray, vec4 min_, vec4 max_);
bool intersectsAABB(inout Ray ray, vec4 min_, vec4 max_, float tMin, float tMax, bool castingShadows);
bool intersectTriangledObject(inout Ray ray, Object obj);
bool intersectObj(inout Ray ray, Object obj);
bool intersectBVHTree(inout Ray ray, bool castingShadows);


// *************************************************************************
// --------------------------------- LIGHT ---------------------------------
// *************************************************************************
/// #include "light.glsl"
bool castShadowRays(Ray ray);
void getDirectionalLightIllumination(Ray ray, Light globalLight, inout vec4 diffuse, inout vec4 specular);
void getPointLightIllumination(Ray ray, Light pointLight, inout vec4 diffuse, inout vec4 specular);
void getIllumination(Ray ray, inout vec4 diffuse, inout vec4 specular);


// **************************************************************************
// ---------------------------------- MAIN ----------------------------------
// **************************************************************************
vec4 castRay(Ray ray)
{
    vec4 color = vec4(0);
    bool hit = false;
    float colorImpact = 1;
    for (int bounce = 0; bounce < maxRayBounce; bounce++)
    {
        for (int objInd = 0; objInd < objectCount; objInd++)
        {
            if (objects[objInd].data.x == 0) continue;

            intersectObj(ray, objects[objInd]);
        }
        intersectBVHTree(ray, false);

        if (ray.surfaceNormal == vec3(0)) break; // no hit

        hit = true;
        ray.interPoint += ray.surfaceNormal * 0.01;

        Material mat = getMaterial(ray.materialIndex);
        vec4 uvColor = texture(textures[0], vec2(ray.uvPos.x, 1 - ray.uvPos.y));
        if (mat.properties1.x == 1)
        {
            vec4 diffuse, specular;
            getIllumination(ray, diffuse, specular);
            color += colorImpact * (1 - mat.properties2.x) * uvColor * mat.color * diffuse * mat.properties1.y;
            color += specular * mat.properties1.z;
        }
        else
            color += colorImpact * (1 - mat.properties2.x) * uvColor * mat.color;

        colorImpact *= mat.properties2.x;
        if (colorImpact <= 1e-6) break;

        vec3 dir = ray.dir - 2 * dot(ray.dir, ray.surfaceNormal) * ray.surfaceNormal;
        ray = Ray(ray.interPoint, dir, RAY_DEFAULT_ARGS);
    }

    color += colorImpact * bgColor;
    return hit ? color : bgColor;
}

void main()
{
    vec3 right = cameraRotMat[0].xyz;
    vec3 forward = cameraRotMat[1].xyz;
    vec3 up = cameraRotMat[2].xyz;

    vec3 lb = focalDistance * forward - 0.5 * right * screenSize.x - 0.5 * up * screenSize.y;
    float x = gl_FragCoord.x / pixelSize.x * screenSize.x;
    float y = gl_FragCoord.y / pixelSize.y * screenSize.y;
    vec3 rayDir = lb + x * right + y * up;

    float rand = 0.5;
    vec4 color = vec4(0);
    for (int i = 0; i < samplesPerPixel; ++i)
    {
        rand = fract(rand * 1232142);

        vec3 lensOffsetStarting = lensRadius * vec3(random(gl_FragCoord.xy, i * 2) - 0.5, random(gl_FragCoord.xy, i * 2 + 1) - 0.5, 0) * (random(gl_FragCoord.xy, i * 2 + 2) - 0.5) * 2;
        vec3 lensOffset = right * lensOffsetStarting.x + up * lensOffsetStarting.y;
        color += castRay(Ray(cameraPos + lensOffset, normalize(rayDir - lensOffset), RAY_DEFAULT_ARGS));
    }

    color /= samplesPerPixel;
    outColor = color;
}
