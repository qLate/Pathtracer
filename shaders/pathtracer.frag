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
uniform int maxRayBounces;
uniform int samplesPerPixel;
uniform int frame;
uniform int accumFrame;

uniform vec2 pixelSize;
uniform vec2 viewSize;
uniform float focalDistance;
uniform float lensRadius;
uniform vec3 cameraPos;
uniform vec3 bgColor = vec3(0, 0, 0);

#include "intersection.glsl"
#include "light.glsl"

vec3 castRay(Ray ray)
{
    vec3 color = vec3(0);
    vec3 throughput = vec3(1);
    for (int i = 0; i < maxRayBounces; i++)
    {
        if (!intersectWorld(ray))
        {
            color += throughput * bgColor;
            break;
        }

        ray.surfaceNormal = dot(ray.surfaceNormal, ray.dir) < 0 ? ray.surfaceNormal : -ray.surfaceNormal;
        ray.interPoint += ray.surfaceNormal * 0.0001;

        Material mat = getMaterial(ray.materialIndex);
        vec2 uv = vec2(ray.uvPos.x, 1 - ray.uvPos.y);
        vec3 albedo = texture(textures[int(mat.textureIndex)], uv).xyz * mat.color;

        if (length(mat.emission) > 0)
        {
            color += throughput * mat.emission;
            break;
        }

        vec3 directLighting = getIllumination(ray);
        color += directLighting * albedo / PI * throughput;

        // vec3 samp = sampleHemisphereUniform(rand(), rand());
        // float cosTheta = samp.z;
        // float pdf = 1 / (2 * PI);
        // throughput *= albedo / (pdf * PI) * cosTheta;

        vec3 samp = sampleHemisphereCosine(rand(), rand());
        // float cosTheta = samp.z;
        // float pdf = cosTheta / PI;
        throughput *= albedo;

        if (length(throughput) < 0.01) break;

        // Russian roulette
        if (i > 3)
        {
            float p = clamp(max(throughput.r, max(throughput.g, throughput.b)), 0.05, 1.0);
            if (rand() > p) break;
            throughput /= p;
        }

        vec3 bounceDir = worldToTangent(samp, ray.surfaceNormal);
        ray = Ray(ray.interPoint, bounceDir, RAY_DEFAULT_ARGS);
    }

    return color;
}

void trace(out vec3 mean, out vec3 m2)
{
    vec3 right = cameraRotMat[0].xyz;
    vec3 forward = cameraRotMat[1].xyz;
    vec3 up = cameraRotMat[2].xyz;

    vec3 lb = focalDistance * forward - 0.5 * viewSize.x * right - 0.5 * viewSize.y * up;
    float dx = viewSize.x / pixelSize.x;
    float dy = viewSize.y / pixelSize.y;
    float x = gl_FragCoord.x * dx;
    float y = gl_FragCoord.y * dy;

    mean = vec3(0.0);
    m2 = vec3(0.0);

    for (int i = 0; i < samplesPerPixel; ++i)
    {
        vec2 jitter = vec2(rand(), rand()) - 0.5;
        vec3 dir = normalize(lb + (x + jitter.x * dx) * right + (y + jitter.y * dy) * up);
        vec3
        sample_ = castRay(Ray(cameraPos, dir, RAY_DEFAULT_ARGS));

        vec3 delta = sample_ - mean;
        mean += delta / float(i + 1);
        vec3 delta2 = sample_ - mean;
        m2 += delta * delta2;
    }
}

uniform sampler2D accumTexture;
uniform sampler2D accumM2Texture;

layout(location = 1) out vec4 outM2;
layout(location = 2) out vec4 outVariance;

void main()
{
    InitRNG(gl_FragCoord.xy, frame);

    vec3 frameMean, frameM2;
    trace(frameMean, frameM2);
    float frameSampleCount = float(samplesPerPixel);

    vec2 uv = gl_FragCoord.xy / pixelSize;
    vec3 prevMean = texture(accumTexture, uv).rgb;
    vec3 prevM2 = texture(accumM2Texture, uv).rgb;

    float frameIndex = float(accumFrame + 1); // starts at 1

    vec3 delta = frameMean - prevMean;
    vec3 newMean = prevMean + delta / frameIndex;
    vec3 delta2 = frameMean - newMean;

    vec3 newM2 = prevM2 + frameM2 + delta * delta2 * frameSampleCount * (frameIndex - 1.0) / frameIndex;
    vec3 variance = newM2 / (frameIndex * frameSampleCount); // variance per pixel sample

    outM2 = vec4(newM2, 1.0);
    outVariance = vec4(variance, 1.0); // optional, for debug display

    if (COLOR_DEBUG != vec3(0))
        outColor = vec4(COLOR_DEBUG, 1.0);
    else
        outColor = vec4(newMean + COLOR_HEAT, 1.0);
}
