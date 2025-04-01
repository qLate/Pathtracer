vec2 genRandoms(int bounce)
{
    float r1, r2;
    // if (bounce == 0) // apply stratified sampling if first bounce
    // {
    //     int strata = int(ceil(sqrt(samplesPerPixel)));
    //     int j = totalSamples % (strata * strata);
    //     r1 = (j % strata + rand()) / strata;
    //     r2 = (j / strata + rand()) / strata;
    // }
    // else
    {
        r1 = rand();
        r2 = rand();
    }
    return vec2(r1, r2);
}

float getTriangleLightPdf(Light light, Triangle tri, Object obj, vec3 P, vec3 L, vec3 LP)
{
    vec3 lightNormal = localToGlobalDir(tri.vertices[0].normalV.xyz, obj);
    float LNdotL = max(dot(-L, lightNormal), 0.0);

    float dist = length(LP - P);
    return dist * dist / (LNdotL * light.properties1.y);
}

float getLightPdf(Light light, vec3 P, vec3 L, vec3 LP)
{
    if (light.lightType == LIGHT_TYPE_POINT)
    {
        return -1;
    }
    else if (light.lightType == LIGHT_TYPE_TRIANGLE)
    {
        Triangle tri = triangles[int(light.properties1.x)];
        Object obj = objects[int(tri.info.y)];
        return getTriangleLightPdf(light, tri, obj, P, L, LP);
    }
    return -1;
}

void sampleLight(int lightIndex, vec3 P, out vec3 L, out vec3 radiance, out float dist, out float pdf)
{
    Light light = lights[lightIndex];
    if (light.lightType == LIGHT_TYPE_POINT)
    {
        L = normalize(light.pos - P);
        dist = length(light.pos - P);

        float distImpact = clamp1(pow(clamp0(1 - dist / light.properties1.y), 2));
        radiance = distImpact * light.color * light.properties1.x;
        pdf = -1;
    }
    else if (light.lightType == LIGHT_TYPE_TRIANGLE)
    {
        Triangle tri = triangles[int(light.properties1.x)];
        Object obj = objects[int(tri.info.y)];

        vec3 v0, v1, v2;
        calcGlobalTriVertices(tri, v0, v1, v2);
        vec3 LP = sampleTriangleUniform(v0, v1, v2, rand(), rand());

        L = normalize(LP - P);
        dist = length(LP - P);

        Material mat = getMaterial(obj.materialIndex);
        radiance = mat.emission;

        pdf = getTriangleLightPdf(light, tri, obj, P, L, LP);
    }
}

vec3 getDirectLighting(vec3 N, vec3 V, vec3 P, vec3 diffColor, vec3 specColor, float roughness, int bounce, out float lightPdf)
{
    if (lightCount == 0) return vec3(0);

    vec3 L, radiance;
    float dist;
    int ind = randInt(0, lightCount - 1);
    sampleLight(ind, P, L, radiance, dist, lightPdf);

    float NdotL = max(dot(L, N), 0.0);
    if (NdotL == 0) return vec3(0);

    Ray shadowRay = Ray(P, L, dist - 0.001, RAY_DEFAULT_ARGS_WO_DIST);
    float shadowMult = lightCount * (intersectWorld(shadowRay, true) ? 0.0 : 1.0);

    float tmpPdf;
    vec3 brdf = ggxBRDF(N, L, V, NdotL, roughness, specColor, diffColor, tmpPdf);
    return shadowMult * radiance * brdf * NdotL / (lightPdf + 0.001);
}

float probToSampleDiffuse(vec3 diffColor, vec3 specColor)
{
    float lumDiff = max(0.01, luminance(diffColor));
    float lumSpec = max(0.01, luminance(specColor));
    return lumDiff / (lumDiff + lumSpec);
}
vec3 scatter(vec3 N, vec3 V, vec3 diffColor, vec3 specColor, float roughness, int bounce, inout vec3 throughput, out float brdfPdf)
{
    float probDiff = probToSampleDiffuse(diffColor, specColor);
    if (rand() < probDiff)
    {
        vec2 r = genRandoms(bounce);
        vec3 localDir = sampleHemisphereCosine(r.x, r.y);
        vec3 L = worldToTangent(localDir, N);

        float NdotL = max(dot(N, L), 0.001);
        brdfPdf = probDiff * NdotL / PI;

        throughput *= diffColor / probDiff;
        return L;
    }
    else
    {
        vec2 r = genRandoms(bounce);
        vec3 H = sampleGGXMicrofacet(N, roughness, r.x, r.y);
        vec3 L = normalize(reflect(-V, H));
        float NdotL = max(dot(N, L), 0.001);

        vec3 brdf = ggxSpecBRDF(N, L, V, NdotL, roughness, specColor, brdfPdf);
        brdfPdf *= (1.0 - probDiff);

        throughput *= NdotL * brdf / brdfPdf;
        return L;
    }
}
vec3 getShading(vec3 N, vec3 V, vec3 P, vec3 diffColor, float roughness, float metallic, int bounce, inout vec3 throughput, out vec3 L, out float brdfPdf)
{
    vec3 specColor = mix(vec3(0), diffColor, metallic);

    float lightPdf;
    vec3 directLighting = throughput * getDirectLighting(N, V, P, diffColor, specColor, roughness, bounce, lightPdf);

    L = scatter(N, V, diffColor, specColor, roughness, bounce, throughput, brdfPdf);
    if (brdfPdf < 0.001)
    {
        throughput = vec3(0);
        return vec3(0);
    }

    float lightMis = powerHeuristic(lightPdf, brdfPdf);
    return directLighting * lightMis;
}
