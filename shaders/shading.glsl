float ggxNormalDistribution(float NdotH, float roughness)
{
    float a2 = roughness * roughness;
    float d = NdotH * NdotH * (a2 - 1.0) + 1.0;
    return a2 / (d * d * PI);
}
float ggxSchlickMaskingTerm(float NdotL, float NdotV, float roughness)
{
    float k = roughness * roughness / 2;
    float g_v = NdotV / (NdotV * (1 - k) + k);
    float g_l = NdotL / (NdotL * (1 - k) + k);
    return g_v * g_l;
}
vec3 ggxSchlickFresnel(vec3 f0, float LdotH)
{
    return f0 + (1.0 - f0) * pow(1.0 - LdotH, 5.0);
}
vec3 sampleGGXMicrofacet(vec3 N, float roughness, float r1, float r2)
{
    float a2 = roughness * roughness;
    float phi = TWO_PI * r1;
    float cosTheta = sqrt(clamp0((1.0 - r2) / (1.0 + (a2 * a2 - 1.0) * r2)));
    float sinTheta = sqrt(clamp0(1.0 - cosTheta * cosTheta));

    vec3 H = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    vec3 T, B;
    onb(N, T, B);
    return normalize(H.x * T + H.y * B + H.z * N);
}

// vec3 sampleDiffuse(Ray ray, inout vec3 throughput, vec3 albedo, Material mat, int bounce)
// {
//     vec2 r = genRandoms(bounce);
//     vec3 localDir = sampleHemisphereCosine(r.x, r.y);
//     vec3 samp = worldToTangent(localDir, ray.surfaceNormal);

//     throughput *= albedo;
//     return samp;
// }

// vec3 sampleGlossy(Ray ray, inout vec3 throughput, vec3 albedo, Material mat, int bounce)
// {
//     vec2 r = genRandoms(bounce);
//     vec3 samp = sampleGGX(ray.surfaceNormal, -ray.dir, mat.roughness, r.x, r.y);

//     float cosTheta = max(dot(ray.surfaceNormal, samp), 0.0);
//     float pdf = 1;

//     throughput *= albedo * cosTheta / pdf;
//     return samp;
// }

// vec3 samplePureSpecular(Ray ray, inout vec3 throughput, vec3 albedo, Material mat, int bounce)
// {
//     vec3 samp = reflect(ray.dir, ray.surfaceNormal);

//     throughput *= albedo;
//     return samp;
// }

// vec3 sampleMaterial(Ray ray, inout vec3 throughput, vec3 albedo, Material mat, int bounce)
// {
//     if (mat.roughness == 1.0)
//         return sampleDiffuse(ray, throughput, albedo, mat, bounce);
//     else
//         return sampleGlossy(ray, throughput, albedo, mat, bounce);
// }
