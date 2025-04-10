bool intersectTriangle(inout Ray ray, int triIndex)
{
    Triangle tri = triangles[triIndex];

    vec3 v0 = tri.vertices[0].posU.xyz;
    vec3 v1 = tri.vertices[1].posU.xyz;
    vec3 v2 = tri.vertices[2].posU.xyz;

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

    if (all(greaterThanEqual(uvt, vec4(-0.00001))) && (uvt.z < ray.t))
    {
        ray.t = uvt.z;
        ray.uv = vec2(uvt.x, uvt.y);
        ray.hitPoint = ray.pos + ray.dir * uvt.z;

        return true;
    }
    return false;
}

vec3 getTriangleNormalAt(Object obj, Triangle tri, float u, float v)
{
    vec3 norm1 = tri.vertices[0].normalV.xyz;
    vec3 norm2 = tri.vertices[1].normalV.xyz;
    vec3 norm3 = tri.vertices[2].normalV.xyz;
    return localToGlobalDir(normalize((1 - u - v) * norm1 + u * norm2 + v * norm3), obj);
}
void calcTriIntersectionValues(inout Ray ray)
{
    Triangle tri = triangles[ray.hitTriIndex];
    Object obj = objects[ray.hitObjIndex];

    vec2 uv0 = vec2(tri.vertices[0].posU.w, tri.vertices[0].normalV.w);
    vec2 uv1 = vec2(tri.vertices[1].posU.w, tri.vertices[1].normalV.w);
    vec2 uv2 = vec2(tri.vertices[2].posU.w, tri.vertices[2].normalV.w);
    ray.uv = uv0 + ray.uv.x * (uv1 - uv0) + ray.uv.y * (uv2 - uv0);

    vec3 v0 = localToGlobal(tri.vertices[0].posU.xyz, obj);
    vec3 v1 = localToGlobal(tri.vertices[1].posU.xyz, obj);
    vec3 v2 = localToGlobal(tri.vertices[2].posU.xyz, obj);

    vec3 norm = getTriangleNormalAt(obj, tri, ray.uv.x, ray.uv.y);
    vec3 baseNorm = normalize(cross(v1 - v0, v2 - v0));
    ray.surfaceNormal = dot(baseNorm != vec3(0) ? baseNorm : norm, ray.dir) < 0 ? norm : -norm;
}

bool intersectBVHBottom(int rootNode, inout Ray ray, bool castingShadows);

bool intersectMesh(inout Ray ray, Object obj, bool castingShadows)
{
    vec3 rayPos = ray.pos;
    vec3 rayDir = ray.dir;
    float rayT = ray.t;

    ray.pos = globalToLocal(rayPos, obj);
    ray.dir = globalToLocalDir(rayDir, obj);

    vec3 tVec = rayPos + rayDir * ray.t;
    vec3 tVecLocal = globalToLocal(tVec, obj);
    ray.t = length(tVecLocal - ray.pos);

    bool hit = false;
    int rootNode = int(obj.properties.z);
    if (rootNode != -1)
    {
        if (intersectBVHBottom(rootNode, ray, castingShadows))
            hit = true;
    }
    // else
    // {
    //     COLOR_DEBUG = RED;
    //     for (int i = int(obj.properties.x); i < obj.properties.x + obj.properties.y; i++)
    //     {
    //         if (intersectTriangle(ray, i))
    //         {
    //             hit = true;
    //             ray.hitTriIndex = i;

    //             if (castingShadows) break;
    //         }
    //     }
    // }

    if (hit)
    {
        ray.hitPoint = localToGlobal(ray.hitPoint, obj);
        ray.t = length(ray.hitPoint - rayPos);
    }
    else
        ray.t = rayT;

    ray.pos = rayPos;
    ray.dir = rayDir;
    return hit;
}

bool intersectSphere(inout Ray ray, Object sphere)
{
    float x0, x1;
    vec3 dir = ray.dir;
    vec3 inter = (ray.pos - sphere.pos.xyz);
    float a = dot(dir, dir);
    float b = dot(dir + dir, inter);
    vec3 objScale = getTransformScale(sphere.transform);
    float c = abs(dot(inter, inter)) - sphere.properties.x * sphere.properties.x * objScale.x * objScale.x;

    if (!solveQuadratic(a, b, c, x0, x1)) return false;
    if (x0 <= 0 || x0 >= ray.t) return false;

    ray.t = x0;
    ray.hitPoint = ray.pos + x0 * dir;
    ray.surfaceNormal = normalize(ray.hitPoint - sphere.pos.xyz);

    vec3 uvN = globalToLocalDir(ray.surfaceNormal, sphere);
    float u = atan(uvN.z, uvN.x) / (2.0 * PI) + 0.5;
    float v = uvN.y * 0.5 + 0.5;
    ray.uv = vec2(u, v);

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
    if (x0 <= 0 || x0 >= ray.t) return false;

    ray.t = x0;
    ray.hitPoint = ray.pos + x0 * dir;
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
    if (t >= ray.t || t <= 0) return false;

    ray.t = t;
    ray.hitPoint = ray.pos + t * ray.dir;
    ray.surfaceNormal = dot(ray.dir, normal) < 0 ? normal : -normal;

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
            ray.hitPoint = point;
            ray.uv = vec2(0.01);
            ray.t = t;
            return true;
        }
    }

    return false;
}

bool intersectsAABB(inout Ray ray, vec4 min_, vec4 max_, float tMin, float tMax, bool castingShadows, bool preventHeat = false)
{
    #ifdef SHOW_BVH_BOXES
    if (!castingShadows && !preventHeat)
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

bool intersectObj(inout Ray ray, Object obj, bool castingShadows)
{
    if (obj.objType == OBJ_TYPE_MESH)
    {
        if (intersectMesh(ray, obj, castingShadows)) return true;
    }
    else if (obj.objType == OBJ_TYPE_SPHERE)
    {
        if (intersectSphere(ray, obj))
        {
            ray.hitTriIndex = -1;
            return true;
        }
    }
    else if (obj.objType == OBJ_TYPE_PLANE)
    {
        if (intersectPlane(ray, obj))
        {
            ray.hitTriIndex = -1;
            return true;
        }
    }
    return false;
}

bool intersectBVHBottom(int rootNode, inout Ray ray, bool castingShadows)
{
    bool hit = false;
    int curr = rootNode;
    int c = 0;
    while (curr != -1)
    {
        BVHNode node = nodes[curr];
        if (intersectsAABB(ray, node.min, node.max, 0, FLT_MAX, castingShadows, true))
        {
            if (node.values.z == 1)
            {
                int triInd = int(node.min.w);
                if (intersectTriangle(ray, triInd))
                {
                    hit = true;
                    ray.hitTriIndex = triInd;

                    if (castingShadows) return true;
                }
            }
            curr = node.links.x;
        }
        else
            curr = node.links.y;

        // if (c++ >= triCount * 2 + 100)
        // {
        //     COLOR_DEBUG = GREEN;
        //     break;
        // }
    }
    return hit;
}

uniform int bvhRootNode;
bool intersectBVHTop(inout Ray ray, bool castingShadows)
{
    bool hit = false;
    int curr = bvhRootNode;
    int c = 0;
    while (curr != -1)
    {
        BVHNode node = nodes[curr];
        if (intersectsAABB(ray, node.min, node.max, 0, FLT_MAX, castingShadows))
        {
            if (node.values.z == 1)
            {
                int objInd = int(node.min.w);
                if (intersectObj(ray, objects[objInd], castingShadows))
                {
                    hit = true;
                    ray.hitObjIndex = objInd;

                    if (castingShadows) return true;
                }
            }
            curr = node.links.x;
        }
        else
            curr = node.links.y;

        // if (c++ >= triCount * 2 + 100)
        // {
        //     COLOR_DEBUG = GREEN;
        //     break;
        // }
    }
    return hit;
}

bool intersectWorld(inout Ray ray, bool castingShadows)
{
    // bool hit = false;
    // for (int i = 0; i < objectCount; i++)
    // {
    //     if (intersectObj(ray, objects[i], castingShadows))
    //     {
    //         hit = true;
    //         ray.hitObjIndex = i;

    //         if (castingShadows) return true;
    //     }
    // }
    // return hit;
    return intersectBVHTop(ray, castingShadows);
}
