float ggxNormalDistribution(float NdotH, float roughness)
{
    if (roughness < 1e-4) return 1.0;
    float a2 = roughness * roughness;
    float d = NdotH * NdotH * (a2 - 1.0) + 1.0;
    return a2 / (d * d * PI + 0.001);
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
    if (roughness < 1e-4) return N;

    float a2 = roughness * roughness;
    float phi = TWO_PI * r1;
    float cosTheta = sqrt(clamp0((1.0 - r2) / (1.0 + (a2 * a2 - 1.0) * r2)));
    float sinTheta = sqrt(clamp0(1.0 - cosTheta * cosTheta));

    vec3 H = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    vec3 T, B;
    onb(N, T, B);
    return normalize(H.x * T + H.y * B + H.z * N);
}

vec3 ggxSpecBRDF(vec3 N, vec3 L, vec3 V, float NdotL, float roughness, vec3 specColor, out float pdf)
{
    vec3 H = normalize(V + L);
    float NdotV = clamp0(dot(N, V));
    float NdotH = clamp0(dot(N, H));
    float LdotH = clamp0(dot(L, H));

    float D = ggxNormalDistribution(NdotH, roughness);
    float G = ggxSchlickMaskingTerm(NdotL, NdotV, roughness);
    vec3 F = ggxSchlickFresnel(specColor, LdotH);

    pdf = D * NdotH / (4.0 * LdotH + 0.001);
    return (D * G * F) / (4.0 * NdotV * NdotL + 0.001);
}
vec3 ggxBRDF(vec3 N, vec3 L, vec3 V, float NdotL, float roughness, vec3 specColor, vec3 diffColor, out float pdf)
{
    vec3 specBrdf = ggxSpecBRDF(N, L, V, NdotL, roughness, specColor, pdf);
    return specBrdf + diffColor / PI;
}
