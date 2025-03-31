#version 460 core
#extension GL_ARB_shading_language_include : enable
#include "common.glsl"

layout(std140, binding = 6) /*buffer*/ uniform BVHNodes
{
    BVHNode nodes[];
};

layout(std430, binding = 7) /*buffer*/ uniform BVHTriIndices
{
    int triIndices[];
};

out vec4 outColor;
vec3 COLOR_HEAT = vec3(0);

// ----------- OPTIONS -----------
// #define SHOW_BVH_BOXES
// #define SHOW_BVH_HEAT

// ----------- SETTINGS -----------
uniform int maxRayBounces;
uniform int samplesPerPixel;

uniform vec2 pixelSize;
uniform vec2 viewSize;
uniform float focalDistance;
uniform float lensRadius;
uniform vec3 cameraPos;
uniform vec3 bgColor = vec3(0, 0, 0);

uniform int frame;
uniform int sampleFrame;
uniform int totalSamples;

#include "intersection.glsl"
#include "shading.glsl"
#include "light.glsl"

vec3 castRay(Ray ray)
{
    vec3 color = vec3(0);
    vec3 throughput = vec3(1);
    for (int bounce = 0; bounce <= maxRayBounces; bounce++)
    {
        if (!intersectWorld(ray))
        {
            color += throughput * bgColor;
            break;
        }

        ray.surfaceNormal = dot(ray.surfaceNormal, ray.dir) < 0 ? ray.surfaceNormal : -ray.surfaceNormal;
        ray.interPoint += ray.surfaceNormal * 0.01;

        Material mat = getMaterial(ray.materialIndex);
        if (length(mat.emission) > 0)
        {
            if (bounce == 0)
                color += throughput * mat.emission;

            if (bounce == 0)
                color = clamp(color, 0, 1);
            break;
        }

        vec2 uv = vec2(ray.uvPos.x, 1 - ray.uvPos.y);
        vec3 albedo = texture(textures[int(mat.textureIndex)], uv).xyz * mat.color;

        vec3 bounceDir;
        color += getShading(ray.surfaceNormal, -ray.dir, ray.interPoint, albedo, mat.roughness, mat.metallic, bounce, throughput, bounceDir);

        if (length(throughput) < 0.01) break;
        ray = Ray(ray.interPoint, bounceDir, RAY_DEFAULT_ARGS);

        // Russian roulette
        if (bounce > 3)
        {
            float p = clamp(maxv3(throughput), 0.05, 1.0);
            if (rand() > p) break;
            throughput /= p;
        }
    }

    return color;
}

vec3 trace()
{
    vec3 right = cameraRotMat[0].xyz;
    vec3 forward = cameraRotMat[1].xyz;
    vec3 up = cameraRotMat[2].xyz;

    vec3 lb = focalDistance * forward - 0.5 * viewSize.x * right - 0.5 * viewSize.y * up;
    float dx = viewSize.x / pixelSize.x;
    float dy = viewSize.y / pixelSize.y;
    float x = gl_FragCoord.x * dx;
    float y = gl_FragCoord.y * dy;
    vec3 rayDir = normalize(lb + x * right + y * up);

    #ifdef BENCHMARK_BUILD
    vec2 jitter = vec2(0, 0);
    #else
    vec2 jitter = vec2(rand(), rand()) - 0.5;
    #endif

    vec3 finalRayDir = normalize(lb + (x + jitter.x * dx) * right + (y + jitter.y * dy) * up);
    return castRay(Ray(cameraPos, finalRayDir, RAY_DEFAULT_ARGS));
}

uniform sampler2D accumMeanTexture;
uniform sampler2D accumSqrTexture;

layout(location = 1) out vec4 outMean;
layout(location = 2) out vec4 outSqr;
layout(location = 3) out vec4 outVariance;

void main()
{
    InitRNG(gl_FragCoord.xy, frame * samplesPerPixel + totalSamples);

    vec3 color = trace();

    vec3 finalColor;
    #ifdef BENCHMARK_BUILD
    {
        finalColor = color;
    }
    #else
    {
        vec2 uv = gl_FragCoord.xy / pixelSize;
        vec3 prevMean = texture(accumMeanTexture, uv).rgb;
        vec3 prevSqr = texture(accumSqrTexture, uv).rgb;

        // Catch NaNs
        if (prevMean != prevMean) prevMean = vec3(0);
        if (prevSqr != prevSqr) prevSqr = vec3(0);

        vec3 newMean = mix(prevMean, color, 1.0 / (totalSamples + 1));
        vec3 newSqr = mix(prevSqr, color * color, 1.0 / (totalSamples + 1));
        vec3 variance = newSqr - newMean * newMean;

        // if (COLOR_DEBUG != vec3(0))
        //     newMean = COLOR_DEBUG;

        outMean = vec4(newMean, 1.0);
        outSqr = vec4(newSqr, 1.0);
        outVariance = vec4(variance, 1.0);

        finalColor = newMean;
    }
    #endif

    // gamma correction
    finalColor = linearToGamma(finalColor);

    outColor = vec4(finalColor, 1);

    if (COLOR_DEBUG != vec3(0))
        outColor = vec4(COLOR_DEBUG, 1);
    if (COLOR_HEAT != vec3(0))
        outColor.xyz += COLOR_HEAT;
}
