#define MIN_ROUGHNESS 0.001

float ggxNormalDistribution(float NdotH, float roughness)
{
    float a2 = roughness * roughness;
    float d = NdotH * NdotH * (a2 - 1.0) + 1.0;
    return a2 / max(d * d * PI, EPSILON);
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
    if (roughness < 0.02) return N;
    float a2 = roughness * roughness;
    float phi = TWO_PI * r1;
    float cosTheta = sqrt(clamp0((1.0 - r2) / (1.0 + (a2 - 1.0) * r2)));
    float sinTheta = sqrt(clamp0(1.0 - cosTheta * cosTheta));

    vec3 H = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    vec3 T, B;
    onb(N, T, B);
    return normalize(H.x * T + H.y * B + H.z * N);
}

vec3 ggxSpecBRDF(vec3 N, vec3 L, vec3 V, float NdotL, float roughness, vec3 specColor)
{
    roughness = clamp(roughness, MIN_ROUGHNESS, 1.0);

    vec3 H = normalize(V + L);
    float NdotV = clamp01(dot(N, V));
    float NdotH = clamp01(dot(N, H));
    float LdotH = clamp01(dot(L, H));

    float D = ggxNormalDistribution(NdotH, roughness);
    float G = ggxSchlickMaskingTerm(NdotL, NdotV, roughness);
    vec3 F = ggxSchlickFresnel(specColor, LdotH);

    return (D * G * F) / max(4.0 * NdotV * NdotL, EPSILON);
}

vec3 ggxBRDF(vec3 N, vec3 L, vec3 V, float NdotL, float roughness, vec3 specColor, vec3 diffColor)
{
    vec3 specBrdf = ggxSpecBRDF(N, L, V, NdotL, roughness, specColor);
    return specBrdf + diffColor / PI;
}

float ggxSpecPdf(vec3 N, vec3 L, vec3 V, float roughness)
{
    roughness = clamp(roughness, MIN_ROUGHNESS, 1.0);

    vec3 H = normalize(V + L);
    float NdotH = clamp0(dot(N, H));
    float LdotH = clamp0(dot(L, H));

    float D = ggxNormalDistribution(NdotH, roughness);
    return (D * NdotH) / max(4.0 * LdotH, EPSILON);
}
