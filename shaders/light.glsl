bool castShadowRays(Ray ray)
{
    for (int objInd = 0; objInd < objectCount; objInd++)
    {
        if (objects[objInd].data.x != 0 && intersectObj(ray, objects[objInd])) return true;
    }
    return intersectBVHTree(ray, true);
}

void getDirectionalLightIllumination(Ray ray, Light globalLight, inout vec4 diffuse)
{
    if (castShadowRays(Ray(ray.interPoint, globalLight.properties1.yzw, RAY_DEFAULT_ARGS))) return;

    float light = max(dot(globalLight.properties1.yzw, ray.surfaceNormal), 0.0);
    diffuse += light * globalLight.color * globalLight.properties1.x;
}

void getPointLightIllumination(Ray ray, Light pointLight, inout vec4 diffuse)
{
    vec3 dir = pointLight.pos.xyz - ray.interPoint;
    float dist = length(dir);
    if (dist > pointLight.properties1.y) return;

    dir = normalize(dir);
    if (sign(dot(ray.surfaceNormal, dir)) == sign(dot(ray.surfaceNormal, ray.dir))) return;
    if (castShadowRays(Ray(pointLight.pos.xyz, -dir, dist, RAY_DEFAULT_ARGS_WO_DIST))) return;

    float distanceImpact = min(pow(1 - dist / pointLight.properties1.y, 2), 1.);
    float lightFacingAtPoint = max(dot(dir, ray.surfaceNormal), 0.0);
    diffuse += (distanceImpact * lightFacingAtPoint) * pointLight.color * pointLight.properties1.x;
}

void getIllumination(Ray ray, inout vec4 diffuse)
{
    diffuse = vec4(0);
    for (int i = 0; i < lightCount; i++)
    {
        if (lights[i].lightType == 0)
            getDirectionalLightIllumination(ray, lights[i], diffuse);
        else if (lights[i].lightType == 1)
            getPointLightIllumination(ray, lights[i], diffuse);
    }
}
