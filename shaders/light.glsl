vec3 getDirectionalLightIllumination(Ray ray, Light globalLight)
{
    Ray shadowRay = Ray(ray.interPoint, globalLight.properties1.yzw, RAY_DEFAULT_ARGS);
    float shadowMult = intersectWorld(shadowRay, true) ? 0.0 : 1.0;

    // mat4x4 rotMat = getTransformRotation(objects[0].transform);
    mat4x4 rotMat = mat4x4(1.0);
    vec3 dir = (rotMat * vec4(globalLight.properties1.yzw, 1.0)).xyz;
    float light = max(dot(dir, ray.surfaceNormal), 0.0);
    return shadowMult * light * globalLight.color * globalLight.properties1.x;
}

vec3 getPointLightIllumination(Ray ray, Light pointLight)
{
    float dist = length(pointLight.pos - ray.interPoint);

    vec3 lightDir = normalize(pointLight.pos - ray.interPoint);
    Ray shadowRay = Ray(pointLight.pos, -lightDir, dist, RAY_DEFAULT_ARGS_WO_DIST);
    float shadowMult = intersectWorld(shadowRay, true) ? 0.0 : 1.0;

    float disImpact = min(pow(clamp0(1 - dist / pointLight.properties1.y), 2), 1);
    return shadowMult * disImpact * pointLight.color * pointLight.properties1.x;
}

vec3 getIllumination(Ray ray)
{
    if (lightCount == 0) return vec3(0);

    Light light = lights[randInt(0, lightCount - 1)];
    vec3 lightDir = normalize(light.pos - ray.interPoint);
    float cosTheta = max(dot(lightDir, ray.surfaceNormal), 0.0);
    if (cosTheta == 0) return vec3(0);

    if (light.lightType == LIGHT_TYPE_GLOBAL)
        return getDirectionalLightIllumination(ray, light) * cosTheta;
    else if (light.lightType == LIGHT_TYPE_POINT)
        return getPointLightIllumination(ray, light) * cosTheta;

    return vec3(0);
}
