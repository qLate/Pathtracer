vec3 getTriangleNormalAt(Triangle tri, float u, float v)
{
    Object obj = objects[int(tri.info.y)];
    vec3 norm1 = localToGlobalDir(tri.vertices[0].normalV.xyz, obj);
    vec3 norm2 = localToGlobalDir(tri.vertices[1].normalV.xyz, obj);
    vec3 norm3 = localToGlobalDir(tri.vertices[2].normalV.xyz, obj);
    return normalize((1 - u - v) * norm1 + u * norm2 + v * norm3);
}

// bool intersectTriangle1(inout Ray ray, Triangle tri)
// {
//     float dz = dot(tri.rows[2].xyz, ray.dir);
//     if (dz == 0) return false;

//     float oz = dot(tri.rows[2].xyz, ray.pos) + tri.rows[2].w;
//     float t = -oz / dz;
//     if (t < 0.0 || t > ray.t || t > ray.maxDis) return false;

//     vec3 hitPos = ray.pos + ray.dir * t;
//     float u = dot(tri.rows[0].xyz, hitPos) + tri.rows[0].w;
//     if (u < 0.0 || u > 1.0) return false;

//     float v = dot(tri.rows[1].xyz, hitPos) + tri.rows[1].w;
//     if (v < 0.0 || u + v > 1.0) return false;

//     ray.t = t;
//     ray.materialIndex = int(tri.info.x);
//     ray.surfaceNormal = getTriangleNormalAt(tri, u, v);
//     ray.interPoint = hitPos;

//     vec2 uv0 = vec2(tri.vertices[0].posU.w, tri.vertices[0].normalV.w);
//     vec2 uv1 = vec2(tri.vertices[1].posU.w, tri.vertices[1].normalV.w);
//     vec2 uv2 = vec2(tri.vertices[2].posU.w, tri.vertices[2].normalV.w);
//     ray.uvPos = uv0 + u * (uv1 - uv0) + v * (uv2 - uv0);

//     return true;
// }
bool intersectTriangle2(inout Ray ray, Triangle tri)
{
    Object obj = objects[int(tri.info.y)];
    vec3 v0 = localToGlobal(tri.vertices[0].posU.xyz, obj);
    vec3 v1 = localToGlobal(tri.vertices[1].posU.xyz, obj);
    vec3 v2 = localToGlobal(tri.vertices[2].posU.xyz, obj);

    vec3 p0 = v0;
    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;

    vec3 pv = cross(ray.dir, e2);
    float det = dot(e1, pv);
    vec3 tv = ray.pos - p0;
    vec3 qv = cross(tv, e1);

    vec4 uvt;
    uvt.x = dot(tv, pv);
    uvt.y = dot(ray.dir, qv);
    uvt.z = dot(e2, qv);
    uvt.xyz = uvt.xyz / det;
    uvt.w = 1.0 - uvt.x - uvt.y;

    if (all(greaterThanEqual(uvt, vec4(-0.00001))) && (uvt.z < ray.t && uvt.z <= ray.maxDis)) {
        ray.t = uvt.z;
        ray.materialIndex = int(obj.materialIndex);
        ray.surfaceNormal = getTriangleNormalAt(tri, uvt.x, uvt.y);
        ray.interPoint = ray.pos + ray.dir * uvt.z;

        vec2 uv0 = vec2(tri.vertices[0].posU.w, tri.vertices[0].normalV.w);
        vec2 uv1 = vec2(tri.vertices[1].posU.w, tri.vertices[1].normalV.w);
        vec2 uv2 = vec2(tri.vertices[2].posU.w, tri.vertices[2].normalV.w);
        ray.uvPos = uv0 + uvt.x * (uv1 - uv0) + uvt.y * (uv2 - uv0);

        return true;
    }
    return false;
}

bool intersectSphere(inout Ray ray, Object sphere)
{
    float x0, x1;
    vec3 dir = ray.dir;
    vec3 inter = (ray.pos - sphere.pos.xyz);
    float a = dot(dir, dir);
    float b = dot(dir + dir, inter);
    vec3 objScale = getTransformScale(sphere.transform);
    float c = abs(dot(inter, inter)) - sphere.properties.x * objScale.x * objScale.x;

    if (!solveQuadratic(a, b, c, x0, x1)) return false;
    if (x0 <= 0 || x0 >= ray.t || x0 >= ray.maxDis) return false;

    ray.t = x0;
    ray.interPoint = ray.pos + x0 * dir;
    ray.surfaceNormal = normalize(ray.interPoint - sphere.pos.xyz);
    ray.materialIndex = int(sphere.materialIndex);

    vec3 n = ray.surfaceNormal;
    float u = atan(-n.x, n.y) / (2.0 * PI) + 0.5;
    float v = -n.z * 0.5 + 0.5;
    ray.uvPos = vec2(u, v);

    return true;
}

bool intersectLight(inout Ray ray, Light light, float radius)
{
    float x0, x1;
    vec3 dir = ray.dir;
    vec3 inter = (ray.pos - light.pos.xyz);
    float a = dot(dir, dir);
    float b = dot(dir + dir, inter);
    float c = abs(dot(inter, inter)) - radius * radius;

    if (!solveQuadratic(a, b, c, x0, x1)) return false;
    if (x0 <= 0 || x0 >= ray.t || x0 >= ray.maxDis) return false;

    ray.t = x0;
    ray.interPoint = ray.pos + x0 * dir;
    ray.surfaceNormal = vec3(1, 0, 0);

    return true;
}

bool intersectPlane(inout Ray ray, Object plane)
{
    vec3 normal = plane.properties.xyz;
    normal = localToGlobalDir(normal, plane);
    float denom = -dot(normal, ray.dir);
    if (denom == 0) return false;

    vec3 dir = plane.pos.xyz - ray.pos;
    float t = -dot(normal, dir) / denom;
    if (t >= ray.t || t <= 0 || t >= ray.maxDis) return false;

    ray.t = t;
    ray.interPoint = ray.pos + t * ray.dir;
    ray.surfaceNormal = dot(ray.dir, normal) < 0 ? normal : -normal;
    ray.materialIndex = int(plane.materialIndex);

    return true;
}

const float boxLineWidth = 0.02;
bool intersectAABBForGizmo(inout Ray ray, vec4 min_, vec4 max_)
{
    float tMin = 0, tMax = FLT_MAX;
    for (int i = 0; i < 3; i++)
    {
        float invD = 1.0 / ray.dir[i];
        float t0 = (min_[i] - ray.pos[i]) * invD;
        float t1 = (max_[i] - ray.pos[i]) * invD;
        if (invD < 0.0)
        {
            float temp = t0;
            t0 = t1;
            t1 = temp;
        }

        tMin = max(t0, tMin);
        tMax = min(t1, tMax);
        if (tMax <= tMin) return false;
    }

    vec2 ts = vec2(tMin, tMax);
    float width = boxLineWidth * (1 + tMax / 15);
    for (int i = 0; i < 2; i++)
    {
        float t = ts[i];
        vec3 point = ray.pos + t * ray.dir;
        if (ray.t > t && ((abs(min_.x - point.x) <= width || abs(max_.x - point.x) <= width ||
                    abs(min_.y - point.y) <= width || abs(max_.y - point.y) <= width) &&
                    (abs(min_.y - point.y) <= width || abs(max_.y - point.y) <= width ||
                        abs(min_.z - point.z) <= width || abs(max_.z - point.z) <= width) &&
                    (abs(min_.z - point.z) <= width || abs(max_.z - point.z) <= width ||
                        abs(min_.x - point.x) <= width || abs(max_.x - point.x) <= width)))
        {
            ray.surfaceNormal = vec3(0, 0, 1);
            ray.interPoint = point;
            ray.uvPos = vec2(0.01);
            ray.t = t;
            ray.materialIndex = 2; // Gizmo material
            return true;
        }
    }

    return false;
}

bool intersectsAABB(inout Ray ray, vec4 min_, vec4 max_, float tMin, float tMax, bool castingShadows)
{
    #ifdef SHOW_BVH_BOXES
    if (!castingShadows)
        intersectAABBForGizmo(ray, min_, max_);
    #endif

    for (int i = 0; i < 3; i++)
    {
        float invD = 1 / ray.dir[i];
        float t0 = (min_[i] - ray.pos[i]) * invD;
        float t1 = (max_[i] - ray.pos[i]) * invD;
        if (invD < 0.0)
        {
            float temp = t0;
            t0 = t1;
            t1 = temp;
        }

        tMin = max(t0, tMin);
        tMax = min(t1, tMax);
        if (tMax <= tMin)
            return false;
    }

    #ifdef SHOW_BVH_HEAT
    COLOR_HEAT.x += pow(1 - COLOR_HEAT.x, 3) * 0.05;
    #endif
    return true;
}

bool intersectDefaultObj(inout Ray ray, Object obj)
{
    if (obj.objType == OBJ_TYPE_SPHERE)
    {
        if (intersectSphere(ray, obj)) return true;
    }
    else if (obj.objType == OBJ_TYPE_PLANE)
    {
        if (intersectPlane(ray, obj)) return true;
    }
    return false;
}

bool intersectBVHTree(inout Ray ray, bool castingShadows, inout int hitTriIndex)
{
    int curr = 0;
    while (curr != -1)
    {
        BVHNode node = nodes[curr];
        if (intersectsAABB(ray, node.min, node.max, 0, FLT_MAX, castingShadows))
        {
            if (node.values.z == 1)
            {
                for (int i = int(node.min.w); i < node.min.w + node.max.w; i++)
                {
                    if (intersectTriangle2(ray, triangles[int(triIndices[i])])) {
                        hitTriIndex = int(triIndices[i]);

                        if (castingShadows) return true;
                    }
                }
            }
            curr = int(node.links.x);
        }
        else {
            curr = int(node.links.y);
        }
    }
    return ray.surfaceNormal != vec3(0);
}
bool intersectBVHTree(inout Ray ray, bool castingShadows)
{
    int hitTriIndex = -1;
    return intersectBVHTree(ray, castingShadows, hitTriIndex);
}

bool intersectWorld(inout Ray ray, bool castingShadows, inout int hitTriIndex, inout int hitObjIndex)
{
    bool hit = false;
    for (int objInd = 0; objInd < objectCount; objInd++)
    {
        if (objects[objInd].objType == 0) continue;

        if (intersectDefaultObj(ray, objects[objInd])) {
            hit = true;
            hitObjIndex = objInd;

            if (castingShadows) return true;
        }
    }

    if (intersectBVHTree(ray, castingShadows, hitTriIndex))
        hit = true;
    return hit;
}
bool intersectWorld(inout Ray ray, bool castingShadows = false)
{
    int hitTriIndex = -1;
    int hitObjIndex = -1;
    return intersectWorld(ray, castingShadows, hitTriIndex, hitObjIndex);
}
