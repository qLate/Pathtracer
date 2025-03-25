#version 460 core
#extension GL_ARB_shading_language_include : enable
#include "common.glsl"

layout(std140, binding = 6) /*buffer*/ uniform BVHNodes
{
    BVHNode nodes[];
};

layout(std430, binding = 7) /*buffer*/ uniform BVHTriIndices
{
    uint triIndices[];
};

out vec4 outColor;
vec3 COLOR_HEAT = vec3(0);

// ----------- OPTIONS -----------
// #define SHOW_BVH_BOXES
// #define SHOW_BVH_HEAT

// ----------- SETTINGS -----------
uniform int maxRayBounce;
uniform int samplesPerPixel;
uniform int frame;

#include "intersection.glsl"
#include "light.glsl"

uniform vec2 pixelSize;
uniform vec2 screenSize;
uniform float focalDistance;
uniform float lensRadius;
uniform vec3 cameraPos;
uniform vec3 bgColor = vec3(0, 0, 0);

vec3 castRay(Ray ray)
{
    vec3 color = vec3(0);
    vec3 impact = vec3(1);
    for (int i = 0; i < maxRayBounce; i++)
    {
        if (!intersectWorld(ray)) break;
        ray.interPoint += ray.surfaceNormal * 0.0001;

        Material mat = getMaterial(ray.materialIndex);
        vec2 uv = vec2(ray.uvPos.x, 1 - ray.uvPos.y);
        vec3 albedo = texture(textures[int(mat.textureIndex)], uv).xyz * mat.color;

        vec3 directLighting = getIllumination(ray);
        color += directLighting * albedo / PI * impact;

        float pdf = 1 / (2 * PI);
        float r1 = rand(); // cos(theta)
        float r2 = rand();

        vec3 samp = sampleHemisphereUniform(r1, r2);
        vec3 bounceDir = worldToTangent(samp, ray.surfaceNormal);

        impact *= r1 / (pdf * PI) * albedo;
        if (dot(impact, impact) < 0.01) break;
        ray = Ray(ray.interPoint, bounceDir, RAY_DEFAULT_ARGS);
    }

    color += impact * bgColor;
    return color;
}

void main()
{
    InitRNG(gl_FragCoord.xy, frame);

    vec3 right = cameraRotMat[0].xyz;
    vec3 forward = cameraRotMat[1].xyz;
    vec3 up = cameraRotMat[2].xyz;

    vec3 lb = focalDistance * forward - 0.5 * screenSize.x * right - 0.5 * screenSize.y * up;
    float x = gl_FragCoord.x / pixelSize.x * screenSize.x;
    float y = gl_FragCoord.y / pixelSize.y * screenSize.y;
    vec3 rayDir = normalize(lb + x * right + y * up);

    vec3 color = vec3(0);
    for (int i = 0; i < samplesPerPixel; ++i)
    {
        color += castRay(Ray(cameraPos, rayDir, RAY_DEFAULT_ARGS));
    }

    color /= samplesPerPixel;
    outColor = vec4(color + COLOR_HEAT + COLOR_DEBUG, 1);
}
