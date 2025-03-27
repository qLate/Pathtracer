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

    vec3 color = vec3(0);
    for (int i = 0; i < samplesPerPixel; i++)
    {
        vec2 jitter = vec2(rand(), rand()) - 0.5;
        vec3 finalRayDir = normalize(lb + (x + jitter.x * dx) * right + (y + jitter.y * dy) * up);
        color += castRay(Ray(cameraPos, finalRayDir, RAY_DEFAULT_ARGS));
    }
    color /= samplesPerPixel;
    return color;
}

uniform sampler2D accumTexture;
uniform sampler2D accumSqrTexture;

layout(location = 1) out vec4 outSqr;

void main()
{
    InitRNG(gl_FragCoord.xy, frame);

    vec3 color = trace();

    vec3 prevMeanColor = texture(accumTexture, gl_FragCoord.xy / pixelSize).rgb;
    vec3 prevSqrColor = texture(accumSqrTexture, gl_FragCoord.xy / pixelSize).rgb;

    float blendFactor = 1.0 / (accumFrame + 1);
    vec3 newMeanColor = mix(prevMeanColor, color, blendFactor);
    vec3 newSqrColor = mix(prevSqrColor, color * color, blendFactor);

    outSqr = vec4(newSqrColor, 1);

    if (COLOR_DEBUG != vec3(0))
        outColor = vec4(COLOR_DEBUG, 1);
    else
        outColor = vec4(newMeanColor + COLOR_HEAT, 1);
}
