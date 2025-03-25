bool castShadowRays(Ray ray)
{
    for (int i = 0; i < objectCount; i++)
    {
        if (objects[i].objType != 0 && intersectDefaultObj(ray, objects[i])) return true;
    }
    return intersectBVHTree(ray, true);
}

void getDirectionalLightIllumination(Ray ray, Light globalLight, inout vec3 diffuse)
{
    if (castShadowRays(Ray(ray.interPoint, globalLight.properties1.yzw, RAY_DEFAULT_ARGS))) return;

    // mat4x4 rotMat = getTransformRotation(objects[0].transform);
    mat4x4 rotMat = mat4x4(1.0);
    vec3 dir = (rotMat * vec4(globalLight.properties1.yzw, 1.0)).xyz;
    float light = max(dot(dir, ray.surfaceNormal), 0.0);
    diffuse += light * globalLight.color * globalLight.properties1.x;
}

void getPointLightIllumination(Ray ray, Light pointLight, inout vec3 diffuse)
{
    vec3 dir = pointLight.pos - ray.interPoint;
    float dist = length(dir);
    if (dist > pointLight.properties1.y) return;

    dir = normalize(dir);
    if (sign(dot(ray.surfaceNormal, dir)) == sign(dot(ray.surfaceNormal, ray.dir))) return;
    if (castShadowRays(Ray(pointLight.pos, -dir, dist, RAY_DEFAULT_ARGS_WO_DIST))) return;

    float distanceImpact = min(pow(1 - dist / pointLight.properties1.y, 2), 1.);
    float angleMult = max(dot(dir, ray.surfaceNormal), 0.0);
    diffuse += (distanceImpact * angleMult) * pointLight.color * pointLight.properties1.x;
}

vec3 getIllumination(Ray ray)
{
    vec3 diffuse = vec3(0);
    for (int i = 0; i < lightCount; i++)
    {
        if (lights[i].lightType == 0)
            getDirectionalLightIllumination(ray, lights[i], diffuse);
        else if (lights[i].lightType == 1)
            getPointLightIllumination(ray, lights[i], diffuse);
    }
    return diffuse;
}
