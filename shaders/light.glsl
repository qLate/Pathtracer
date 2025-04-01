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

void sampleLight(int lightIndex, vec3 point, out vec3 L, out vec3 radiance, out float dist)
{
    Light light = lights[lightIndex];

    if (light.lightType == LIGHT_TYPE_POINT)
    {
        L = normalize(light.pos - point);
        dist = length(light.pos - point);

        float distImpact = clamp1(pow(clamp0(1 - dist / light.properties1.y), 2));
        radiance = distImpact * light.color * light.properties1.x;
    }
    else if (light.lightType == LIGHT_TYPE_TRIANGLE)
    {
        Triangle tri = triangles[int(light.properties1.x)];
        Object obj = objects[int(tri.info.y)];

        vec3 v0, v1, v2;
        calcGlobalTriVertices(tri, v0, v1, v2);
        vec3 sample_ = sampleTriangleUniform(v0, v1, v2, rand(), rand());

        L = normalize(sample_ - point);
        dist = length(sample_ - point);

        vec3 lightNormal = localToGlobalDir(tri.vertices[0].normalV.xyz, obj);
        float LNdotL = max(dot(-L, lightNormal), 0.0);

        Material mat = getMaterial(obj.materialIndex);
        radiance = mat.emission * LNdotL / (dist * dist) * light.properties1.y;
    }
}

vec3 getDirectLighting(vec3 N, vec3 V, vec3 P, vec3 diffColor, vec3 specColor, float roughness, int bounce)
{
    if (lightCount == 0) return vec3(0);

    vec3 L, radiance;
    float dist;
    int lightIndex = randInt(0, lightCount - 1);
    sampleLight(lightIndex, P, L, radiance, dist);

    float NdotL = max(dot(L, N), 0.0);
    if (NdotL == 0) return vec3(0);

    Ray shadowRay = Ray(P, L, dist - 0.001, RAY_DEFAULT_ARGS_WO_DIST);
    float shadowMult = lightCount * (intersectWorld(shadowRay, true) ? 0.0 : 1.0);

    float pdf;
    vec3 brdf = ggxBRDF(N, L, V, NdotL, roughness, specColor, diffColor, pdf);
    return shadowMult * radiance * brdf * NdotL;
}

float probToSampleDiffuse(vec3 diffColor, vec3 specColor)
{
    float lumDiff = max(0.01, luminance(diffColor));
    float lumSpec = max(0.01, luminance(specColor));
    return lumDiff / (lumDiff + lumSpec);
}
vec3 scatter(vec3 N, vec3 V, vec3 diffColor, vec3 specColor, float roughness, int bounce, inout vec3 throughput)
{
    float probDiff = probToSampleDiffuse(diffColor, specColor);
    if (rand() < probDiff)
    {
        vec2 r = genRandoms(bounce);
        vec3 localDir = sampleHemisphereCosine(r.x, r.y);
        vec3 L = worldToTangent(localDir, N);

        throughput *= diffColor / probDiff;
        return L;
    }
    else
    {
        vec2 r = genRandoms(bounce);
        vec3 H = sampleGGXMicrofacet(N, roughness, r.x, r.y);
        vec3 L = normalize(reflect(-V, H));

        float NdotL = clamp0(dot(N, L));

        float pdf;
        vec3 brdf = ggxSpecBRDF(N, L, V, NdotL, roughness, specColor, pdf);
        if (pdf < 0.001)
        {
            throughput = vec3(0);
            return vec3(0);
        }

        throughput *= NdotL * brdf / (pdf * (1.0 - probDiff));
        // throughput *= specColor * NdotL / (1 - prob);
        return L;
    }
}

vec3 getShading(vec3 N, vec3 V, vec3 P, vec3 diffColor, float roughness, float metallic, int bounce, inout vec3 throughput, out vec3 L)
{
    vec3 specColor = mix(vec3(0), diffColor, metallic);
    vec3 directLighting = throughput * getDirectLighting(N, V, P, diffColor, specColor, roughness, bounce);

    L = scatter(N, V, diffColor, specColor, roughness, bounce, throughput);
    return directLighting;
}
