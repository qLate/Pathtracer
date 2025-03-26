bool castShadowRays(Ray ray)
{
    for (int i = 0; i < objectCount; i++)
    {
        if (objects[i].objType != 0 && intersectDefaultObj(ray, objects[i])) return true;
    }
    return intersectBVHTree(ray, true);
}

vec3 getDirectionalLightIllumination(Ray ray, Light globalLight)
{
    if (castShadowRays(Ray(ray.interPoint, globalLight.properties1.yzw, RAY_DEFAULT_ARGS))) return vec3(0);

    // mat4x4 rotMat = getTransformRotation(objects[0].transform);
    mat4x4 rotMat = mat4x4(1.0);
    vec3 dir = (rotMat * vec4(globalLight.properties1.yzw, 1.0)).xyz;
    float light = max(dot(dir, ray.surfaceNormal), 0.0);
    return light * globalLight.color * globalLight.properties1.x;
}

vec3 getPointLightIllumination(Ray ray, Light pointLight)
{
    float dist = length(pointLight.pos - ray.interPoint);
    if (dist > pointLight.properties1.y) return vec3(0);

    vec3 lightDir = normalize(pointLight.pos - ray.interPoint);
    if (castShadowRays(Ray(pointLight.pos, -lightDir, dist, RAY_DEFAULT_ARGS_WO_DIST))) return vec3(0);

    float disImpact = min(pow(1 - dist / pointLight.properties1.y, 2), 1);
    return disImpact * pointLight.color * pointLight.properties1.x;
}

vec3 getIllumination(Ray ray)
{
    vec3 diffuse = vec3(0);
    for (int i = 0; i < lightCount; i++)
    {
        Light light = lights[i];
        vec3 lightDir = normalize(light.pos - ray.interPoint);

        float cosTheta = max(dot(lightDir, ray.surfaceNormal), 0.0);
        if (cosTheta == 0) continue;

        if (light.lightType == LIGHT_TYPE_GLOBAL)
            diffuse += getDirectionalLightIllumination(ray, light) * cosTheta;
        else if (light.lightType == LIGHT_TYPE_POINT)
            diffuse += getPointLightIllumination(ray, light) * cosTheta;
    }
    return diffuse;
}
