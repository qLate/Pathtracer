vec2 genRandoms(int bounce)
{
    float r1, r2;
    if (bounce == 0) // apply stratified sampling if first bounce
    {
        int strata = int(ceil(sqrt(samplesPerPixel)));
        int j = totalSamples % (strata * strata);
        r1 = (j % strata + rand()) / strata;
        r2 = (j / strata + rand()) / strata;
    }
    else
    {
        r1 = rand();
        r2 = rand();
    }
    return vec2(r1, r2);
}

vec3 getPointLightRadiance(Light pointLight, vec3 point, float dist)
{
    float distImpact = clamp1(pow(clamp0(1 - dist / pointLight.properties1.y), 2));
    return distImpact * pointLight.color * pointLight.properties1.x;
}

void getLightInfo(int lightIndex, vec3 point, out vec3 lightDir, out vec3 radiance, out float dist)
{
    Light light = lights[lightIndex];
    lightDir = normalize(light.pos - point);
    dist = length(light.pos - point);

    if (light.lightType == LIGHT_TYPE_POINT)
        radiance = getPointLightRadiance(light, point, dist);
}

vec3 ggxDirect(vec3 N, vec3 L, vec3 V, float NdotL, float roughness, vec3 diffColor, vec3 specColor)
{
    vec3 H = normalize(V + L);
    float NdotH = clamp0(dot(N, H));
    float LdotH = clamp0(dot(L, H));
    float NdotV = clamp0(dot(N, V));

    float D = ggxNormalDistribution(NdotH, roughness);
    float G = ggxSchlickMaskingTerm(NdotL, NdotV, roughness);
    vec3 F = ggxSchlickFresnel(specColor, LdotH);

    vec3 ggxTerm = (D * G * F) / (4.0 * NdotV + 0.001);
    return ggxTerm + NdotL * diffColor / PI;
}

vec3 getDirectLighting(vec3 N, vec3 V, vec3 P, vec3 diffColor, vec3 specColor, float roughness, int bounce)
{
    if (lightCount == 0) return vec3(0);

    vec3 lightDir, radiance;
    float dist;
    int lightIndex = randInt(0, lightCount - 1);
    getLightInfo(lightIndex, P, lightDir, radiance, dist);

    float cosTerm = max(dot(lightDir, N), 0.0);
    if (cosTerm == 0) return vec3(0);

    Ray shadowRay = Ray(P, lightDir, dist, RAY_DEFAULT_ARGS_WO_DIST);
    float shadowMult = lightCount * (intersectWorld(shadowRay, true) ? 0.0 : 1.0);

    return shadowMult * radiance * ggxDirect(N, lightDir, V, cosTerm, roughness, diffColor, specColor);
}

float probToSampleDiffuse(vec3 diffColor, vec3 specColor)
{
    float lumDiff = max(0.01, luminance(diffColor));
    float lumSpec = max(0.01, luminance(specColor));
    return lumDiff / (lumDiff + lumSpec);
}
vec3 scatter(vec3 N, vec3 V, vec3 diffColor, vec3 specColor, float roughness, int bounce, inout vec3 throughput)
{
    float prob = probToSampleDiffuse(diffColor, specColor);
    if (rand() < prob)
    {
        vec2 r = genRandoms(bounce);
        vec3 localDir = sampleHemisphereCosine(r.x, r.y);
        vec3 L = worldToTangent(localDir, N);

        throughput *= diffColor / prob;
        return L;
    }
    else
    {
        vec2 r = genRandoms(bounce);
        vec3 H = sampleGGXMicrofacet(N, roughness, r.x, r.y);
        vec3 L = normalize(reflect(-V, H));

        float NdotL = clamp0(dot(N, L));
        float NdotV = clamp0(dot(N, V));
        float NdotH = clamp0(dot(N, H));
        float LdotH = clamp0(dot(L, H));

        float D = ggxNormalDistribution(NdotH, roughness);
        float G = ggxSchlickMaskingTerm(NdotL, NdotV, roughness);
        vec3 F = ggxSchlickFresnel(specColor, LdotH);

        vec3 ggxTerm = (D * G * F) / max(4.0 * NdotV, 0.001);

        float ggxPDF = D * NdotH / max(4.0 * LdotH, 0.001);
        if (ggxPDF < 0.001)
        {
            throughput = vec3(0);
            return vec3(0);
        }

        throughput *= ggxTerm / (ggxPDF * (1.0 - prob));
        // throughput *= specColor * NdotL / (1 - prob);
        return L;
    }
}

vec3 getShading(vec3 N, vec3 V, vec3 P, vec3 diffColor, float roughness, float metallic, int bounce, inout vec3 throughput, out vec3 L)
{
    vec3 specColor = mix(vec3(0.05), diffColor, metallic);
    vec3 directLighting = throughput * getDirectLighting(N, V, P, diffColor, specColor, roughness, bounce);

    L = scatter(N, V, diffColor, specColor, roughness, bounce, throughput);
    return directLighting;
}
