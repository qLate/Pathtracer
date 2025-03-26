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
        if (!intersectWorld(ray)) break;
        ray.surfaceNormal = dot(ray.surfaceNormal, ray.dir) < 0 ? ray.surfaceNormal : -ray.surfaceNormal;
        ray.interPoint += ray.surfaceNormal * 0.0001;

        Material mat = getMaterial(ray.materialIndex);
        vec2 uv = vec2(ray.uvPos.x, 1 - ray.uvPos.y);
        vec3 albedo = texture(textures[int(mat.textureIndex)], uv).xyz * mat.color;

        vec3 directLighting = getIllumination(ray);
        color += directLighting * albedo / PI * throughput;

        // vec3 samp = sampleHemisphereUniform(rand(), rand());
        // float cosTheta = samp.z;
        // float pdf = 1 / (2 * PI);

        vec3 samp = sampleHemisphereCosine(rand(), rand());
        float cosTheta = samp.z;
        float pdf = cosTheta / PI;

        vec3 bounceDir = worldToTangent(samp, ray.surfaceNormal);

        throughput *= albedo / (pdf * PI) * cosTheta;
        if (dot(throughput, throughput) < 0.01) break;

        ray = Ray(ray.interPoint, bounceDir, RAY_DEFAULT_ARGS);
    }

    color += throughput * bgColor;
    return color;
}

uniform sampler2D accumTexture;
void main()
{
    InitRNG(gl_FragCoord.xy, frame);

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
    for (int i = 0; i < samplesPerPixel; ++i)
    {
        vec2 jitter = vec2(rand(), rand());
        vec3 finalRayDir = normalize(lb + (x + jitter.x * dx) * right + (y + jitter.y * dy) * up);
        color += castRay(Ray(cameraPos, finalRayDir, RAY_DEFAULT_ARGS));
    }

    color /= samplesPerPixel;
    color = clamp(color, 0, 1);

    vec4 accumColor = texture(accumTexture, gl_FragCoord.xy / pixelSize);
    accumColor.rgb = mix(accumColor.rgb, color, 1.0f / (accumFrame + 1));

    if (COLOR_DEBUG != vec3(0))
        outColor = vec4(COLOR_DEBUG, 1);
    else
        outColor = vec4(accumColor.rgb + COLOR_HEAT, 1);
}
