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

uniform sampler2D envMap;
uniform bool useEnvMap = false;
uniform mat4x4 envMapToWorld = mat4(1.0);
vec3 sampleEnvMap(vec3 dir)
{
    if (!useEnvMap) return vec3(1);

    dir = normalize((envMapToWorld * vec4(dir, 0)).xyz);
    float u = atan(dir.z, dir.x) / (2.0 * PI) + 0.5;
    float v = acos(clamp(dir.y, -1.0, 1.0)) / PI;
    return texture(envMap, vec2(u, v)).rgb;
}

float getTriangleLightPdf(Light light, Triangle tri, Object obj, vec3 P, vec3 L, vec3 LP)
{
    vec3 LN = localToGlobalDir(tri.vertices[0].normalV.xyz, obj);
    float LNdotL = clamp0(dot(-L, LN));

    float dist = length(LP - P);
    return dist * dist / max(LNdotL * light.properties1.y, EPSILON);
}
float getDiskLightPdf(Light light, Object obj, vec3 P, vec3 L, vec3 LP)
{
    vec3 LN = localToGlobalDir(vec3(0, -1, 0), obj);
    float LNdotL = max(dot(-L, LN), 0.0);

    float dist = length(LP - P);
    float radius = obj.properties.x * getTransformScale(obj.transform).x;
    float area = PI * radius * radius;
    return dist * dist / max(LNdotL * area, EPSILON);
}

float getLightPdf(Light light, Object obj, vec3 P, vec3 L, vec3 LP)
{
    if (light.lightType == LIGHT_TYPE_POINT)
    {
        return -1;
    }
    else if (light.lightType == LIGHT_TYPE_TRIANGLE)
    {
        Triangle tri = triangles[int(light.properties1.x)];
        return getTriangleLightPdf(light, tri, obj, P, L, LP);
    }
    else if (light.lightType == LIGHT_TYPE_DISK)
    {
        return getDiskLightPdf(light, obj, P, L, LP);
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

        float distImpact = light.properties1.y == -1 ?
            1 / (dist * dist) :
            clamp1(pow(clamp0(1 - dist / light.properties1.y), 2));
        radiance = distImpact * light.color * light.properties1.x;
        pdf = 1;
    }
    else if (light.lightType == LIGHT_TYPE_DIRECTIONAL)
    {
        L = -light.properties1.yzw;
        dist = 1e10;
        radiance = light.color * light.properties1.x;
        pdf = 1;
    }
    else if (light.lightType == LIGHT_TYPE_TRIANGLE)
    {
        Triangle tri = triangles[int(light.properties1.x)];
        Object obj = objects[int(light.properties1.z)];

        vec3 v0, v1, v2;
        calcGlobalTriVertices(tri, obj, v0, v1, v2);

        vec3 LP = sampleTriangleUniform(v0, v1, v2, rand(), rand());
        L = normalize(LP - P);
        dist = length(LP - P);

        radiance = findMaterial(obj.materialIndex).emission;
        pdf = getTriangleLightPdf(light, tri, obj, P, L, LP);
    }
    else if (light.lightType == LIGHT_TYPE_DISK)
    {
        Object obj = objects[int(light.properties1.x)];
        float radius = obj.properties.x * getTransformScale(obj.transform).x;

        vec3 circlePoint = sampleCircleCosine(rand(), rand());
        vec3 LP = localToGlobal(circlePoint * radius, obj);
        L = normalize(LP - P);
        dist = length(LP - P);

        radiance = findMaterial(obj.materialIndex).emission;
        pdf = getDiskLightPdf(light, obj, P, L, LP);
    }
    else if (light.lightType == LIGHT_TYPE_ENVIRONMENTAL)
    {
        L = sampleSphereUniform(rand(), rand());
        dist = 1e10;
        radiance = sampleEnvMap(L) * light.color;
        pdf = 1 / (4 * PI);
    }
}

vec3 getDirectLighting(vec3 N, vec3 V, vec3 P, vec3 diffColor, vec3 specColor, float roughness, int bounce, out float lightPdf)
{
    if (lightCount == 0)
    {
        lightPdf = 0;
        return vec3(0);
    }

    vec3 L, radiance;
    float dist;
    int ind = randInt(0, lightCount - 1);
    sampleLight(ind, P, L, radiance, dist, lightPdf);
    lightPdf /= lightCount;

    float NdotL = clamp0(dot(L, N));
    if (NdotL < 1e-5 || lightPdf > 1e15)
    {
        lightPdf = 0;
        return vec3(0);
    }

    Ray shadowRay = Ray(P, L, dist - 0.001, RAY_DEFAULT_ARGS_WO_DIST);
    float shadowMult = intersectWorld(shadowRay, true) ? 0.0 : 1.0;

    vec3 brdf = ggxBRDF(N, L, V, NdotL, roughness, specColor, diffColor);
    return shadowMult * radiance * brdf * NdotL / lightPdf;
}

float probToSampleDiffuse(vec3 diffColor, vec3 specColor, float metallic)
{
    float lumDiff = max(0.01, luminance(diffColor));
    float lumSpec = max(0.01, luminance(specColor));
    float blend = lumDiff / (lumDiff + lumSpec);
    return (1.0 - metallic) * blend;
}

vec3 scatter(vec3 N, vec3 V, vec3 diffColor, vec3 specColor, float roughness, float metallic, int bounce, inout vec3 throughput, out float pdf)
{
    vec2 r = genRandoms(bounce);
    float probDiff = probToSampleDiffuse(diffColor, specColor, metallic);
    if (rand() < probDiff)
    {
        vec3 L_local = sampleHemisphereCosine(r.x, r.y);
        vec3 L = worldToTangent(L_local, N);
        float NdotL = clamp0(dot(N, L));

        throughput *= diffColor / probDiff;
        pdf = NdotL / PI * probDiff;
        return L;
    }
    else
    {
        vec3 H = sampleGGXMicrofacet(N, roughness, r.x, r.y);
        vec3 L = normalize(reflect(-V, H));
        pdf = ggxSpecPdf(N, L, V, roughness) * (1.0 - probDiff);

        float NdotL = clamp0(dot(N, L));
        vec3 brdf = ggxSpecBRDF(N, L, V, NdotL, roughness, specColor);
        if (pdf < 0.001)
        {
            pdf = 0.001;
            throughput = vec3(0);
            L = vec3(0);
        }

        throughput *= brdf * NdotL / pdf;
        return L;
    }
}
vec3 getRadiance(vec3 N, vec3 V, vec3 P, vec3 diffColor, vec3 specColor, float roughness, float metallic, int bounce, inout vec3 throughput, out vec3 bounceDir, out float brdfPdf)
{
    float lightPdf;
    vec3 directLighting = misSampleLight ? getDirectLighting(N, V, P, diffColor, specColor, roughness, bounce, lightPdf) : vec3(0);
    // directLighting = clampMax(directLighting, 1);

    bounceDir = scatter(N, V, diffColor, specColor, roughness, metallic, bounce, throughput, brdfPdf);

    float lightMis = powerHeuristic(lightPdf, misSampleBrdf ? brdfPdf : 0);
    return directLighting * lightMis;
}
