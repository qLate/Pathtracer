vec3 getTriangleNormalAt(Triangle tri, float u, float v, bool invert)
{
    vec3 interpolatedNormal = normalize((1 - u - v) * tri.vertices[0].normalV.xyz + u * tri.vertices[1].normalV.xyz + v * tri.vertices[2].normalV.xyz);
    return invert ? -interpolatedNormal : interpolatedNormal;
}

bool intersectTriangle(inout Ray ray, Triangle tri)
{
    float dz = dot(tri.rows[2].xyz, ray.dir);
    if (dz == 0) return false;

    float oz = dot(tri.rows[2].xyz, ray.pos) + tri.rows[2].w;
    float t = -oz / dz;
    if (t < 0.0 || t > ray.t || t > ray.maxDis) return false;

    vec3 hitPos = ray.pos + ray.dir * t;
    float u = dot(tri.rows[0].xyz, hitPos) + tri.rows[0].w;
    if (u < 0.0 || u > 1.0) return false;

    float v = dot(tri.rows[1].xyz, hitPos) + tri.rows[1].w;
    if (v < 0.0 || u + v > 1.0) return false;

    ray.t = t;
    ray.materialIndex = int(tri.materialIndex.x);
    ray.surfaceNormal = getTriangleNormalAt(tri, u, v, false);
    ray.interPoint = hitPos;

    vec2 uv0 = vec2(tri.vertices[0].posU.w, tri.vertices[0].normalV.w);
    vec2 uv1 = vec2(tri.vertices[1].posU.w, tri.vertices[1].normalV.w);
    vec2 uv2 = vec2(tri.vertices[2].posU.w, tri.vertices[2].normalV.w);
    ray.uvPos = uv0 + u * (uv1 - uv0) + v * (uv2 - uv0);

    return true;
}
// bool intersectTriangle(inout Ray ray, Triangle tri)
// {
//     vec3 V0 = tri.vertices[0].posU.xyz;
//     vec3 V1 = tri.vertices[1].posU.xyz;
//     vec3 V2 = tri.vertices[2].posU.xyz;

//     vec3 p0 = V0;
//     vec3 e0 = V1 - V0;
//     vec3 e1 = V2 - V0;

//     vec3 pv = cross(ray.dir, e1);
//     float det = dot(e0, pv);
//     vec3 tv = ray.pos - p0;
//     vec3 qv = cross(tv, e0);

//     vec4 uvt;
//     uvt.x = dot(tv, pv);
//     uvt.y = dot(ray.dir, qv);
//     uvt.z = dot(e1, qv);
//     uvt.xyz = uvt.xyz / det;
//     uvt.w = 1.0 - uvt.x - uvt.y;

//     if (all(greaterThanEqual(uvt, vec4(0.0))) && (uvt.z < ray.t)) {
//         ray.t = uvt.z;
//         ray.materialIndex = int(tri.materialIndex.x);
//         ray.surfaceNormal = getTriangleNormalAt(tri, uvt.x, uvt.y, false);
//         ray.interPoint = ray.pos + ray.dir * uvt.z;
//         ray.uvPos = vec2(tri.vertices[0].posU.w, tri.vertices[0].normalV.w) + uvt.x * tri.texVec.xy + uvt.y * tri.texVec.zw;
//         return true;
//     }
//     return false;
// }

bool intersectSphere(inout Ray ray, Object sphere)
{
    float x0, x1;
    vec3 dir = ray.dir;
    vec3 inter = (ray.pos - sphere.pos.xyz);
    float a = dot(dir, dir);
    float b = dot(dir + dir, inter);
    float c = abs(dot(inter, inter)) - sphere.properties.x;

    if (!solveQuadratic(a, b, c, x0, x1)) return false;
    if (!(x0 > 0 && x0 < ray.t && x0 < ray.maxDis)) return false;

    ray.t = x0;
    ray.interPoint = ray.pos + x0 * dir;
    ray.surfaceNormal = normalize(ray.interPoint - sphere.pos.xyz);
    ray.materialIndex = int(sphere.data.y);

    vec3 n = ray.surfaceNormal;
    float u = atan(-n.x, n.y) / (2.0 * PI) + 0.5;
    float v = -n.z * 0.5 + 0.5;
    ray.uvPos = vec2(u, v);

    return true;
}

bool intersectPlane(inout Ray ray, Object plane)
{
    vec3 normal = plane.properties.xyz;
    float denom = -dot(normal, ray.dir);
    if (denom <= 1e-6) return false;

    vec3 dir = plane.pos.xyz - ray.pos;
    float t = -dot(normal, dir) / denom;
    if (t >= ray.t || t <= 0 || t >= ray.maxDis) return false;

    ray.t = t;
    ray.interPoint = ray.pos + t * ray.dir;
    ray.surfaceNormal = normal;
    ray.materialIndex = int(plane.data.y);

    return true;
}

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
    float boxLineWidth = boxLineWidth * (1 + tMax / 10);

    vec2 ts = vec2(tMin, tMax);
    for (int i = 0; i < 2; i++)
    {
        float t = ts[i];
        vec3 point = ray.pos + t * ray.dir;
        if (ray.t > t && ((abs(min_.x - point.x) <= boxLineWidth || abs(max_.x - point.x) <= boxLineWidth ||
                    abs(min_.y - point.y) <= boxLineWidth || abs(max_.y - point.y) <= boxLineWidth) &&
                    (abs(min_.y - point.y) <= boxLineWidth || abs(max_.y - point.y) <= boxLineWidth ||
                        abs(min_.z - point.z) <= boxLineWidth || abs(max_.z - point.z) <= boxLineWidth) &&
                    (abs(min_.z - point.z) <= boxLineWidth || abs(max_.z - point.z) <= boxLineWidth ||
                        abs(min_.x - point.x) <= boxLineWidth || abs(max_.x - point.x) <= boxLineWidth)))
        {
            ray.surfaceNormal = vec3(0, 0, 1);
            ray.interPoint = point;
            ray.t = t;
            ray.materialIndex = 0;
            return true;
        }
    }

    return false;
}

bool intersectsAABB(inout Ray ray, vec4 min_, vec4 max_, float tMin, float tMax, bool castingShadows)
{
    #ifdef SHOW_BOXES
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

    #ifdef SHOW_BOXES
    COLOR_ADD.x += pow(1 - COLOR_ADD.x, 3) * 0.05;
    #endif
    return true;
}

bool intersectTriangledObject(inout Ray ray, Object obj)
{
    bool hit = false;
    for (int j = int(obj.properties.x); j < obj.properties.x + obj.properties.y; j++)
    {
        if (intersectTriangle(ray, triangles[j]))
            hit = true;
    }
    return hit;
}

bool intersectObj(inout Ray ray, Object obj)
{
    if (obj.data.x == 0)
    {
        if (intersectTriangledObject(ray, obj)) return true;
    }
    else if (obj.data.x == 1)
    {
        if (intersectSphere(ray, obj)) return true;
    }
    else if (obj.data.x == 2)
    {
        if (intersectPlane(ray, obj)) return true;
    }
    return false;
}

bool intersectBVHTree(inout Ray ray, bool castingShadows)
{
    int curr = 0;
    while (curr != -1)
    {
        BVHNode node = nodes[curr];
        if (intersectsAABB(ray, node.min, node.max, 0, FLT_MAX, castingShadows))
        {
            if (node.values.z == 1)
            {
                for (int i = int(node.min.w); i < node.min.w + node.max.w; i++) intersectTriangle(ray, triangles[i]);
            }
            curr = int(node.values.x);
        }
        else
            curr = int(node.values.y);
    }
    return ray.surfaceNormal != vec3(0);
}
