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

vec3 sampleDiffuse(Ray ray, inout vec3 throughput, vec3 albedo, Material mat, int bounce)
{
    throughput *= albedo;

    vec2 r = genRandoms(bounce);
    vec3 localDir = sampleHemisphereCosine(r.x, r.y);
    return worldToTangent(localDir, ray.surfaceNormal);
}

vec3 sampleGlossy(Ray ray, inout vec3 throughput, vec3 albedo, Material mat, int bounce)
{
    throughput *= albedo;

    vec2 r = genRandoms(bounce);
    return sampleGlossyGGX(ray.surfaceNormal, -ray.dir, mat.roughness, r.x, r.y);
}

vec3 sampleSpecular(Ray ray, inout vec3 throughput, vec3 albedo, Material mat, int bounce)
{
    throughput *= albedo;

    return reflect(ray.dir, ray.surfaceNormal);
}

vec3 sampleMaterial(Ray ray, inout vec3 throughput, vec3 albedo, Material mat, int bounce)
{
    if (mat.roughness == 1.0)
        return sampleDiffuse(ray, throughput, albedo, mat, bounce);
    else
        return sampleGlossy(ray, throughput, albedo, mat, bounce);
}
