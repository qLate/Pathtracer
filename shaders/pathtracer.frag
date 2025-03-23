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
vec4 COLOR_HEAT = vec4(0);

// ----------- OPTIONS -----------
// #define SHOW_BVH_BOXES
// #define SHOW_BVH_HEAT

// ----------- SETTINGS -----------
uniform int maxRayBounce;
uniform int samplesPerPixel = 1;

#include "intersection.glsl"
#include "light.glsl"

uniform vec2 pixelSize;
uniform vec2 screenSize;
uniform float focalDistance;
uniform float lensRadius;
uniform vec3 cameraPos;
uniform vec4 bgColor = vec4(0, 0, 0, 1);

vec4 castRay(Ray ray)
{
    vec4 color = vec4(0);
    bool hit = false;
    float colorImpact = 1;
    for (int bounce = 0; bounce < maxRayBounce; bounce++)
    {
        intersectWorld(ray);

        if (ray.t == FLT_MAX) break; // no hit

        hit = true;
        ray.interPoint += ray.surfaceNormal * 0.01;

        Material mat = getMaterial(ray.materialIndex);
        vec2 uv = vec2(ray.uvPos.x, 1 - ray.uvPos.y);

        vec4 uvColor = texture(textures[int(mat.textureIndex)], uv);
        if (mat.lit)
        {
            vec4 diffuse;
            getIllumination(ray, diffuse);
            color += colorImpact * (1 - mat.reflection) * uvColor * mat.color * diffuse * mat.diffuseCoeff;
        }
        else
            color += colorImpact * (1 - mat.reflection) * uvColor * mat.color;

        colorImpact *= mat.reflection;
        if (colorImpact <= 1e-6) break;

        vec3 dir = reflect(ray.dir, ray.surfaceNormal);
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

    vec3 lb = focalDistance * forward - 0.5 * screenSize.x * right - 0.5 * screenSize.y * up;
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
    outColor = color + COLOR_HEAT + COLOR_DEBUG;
}
