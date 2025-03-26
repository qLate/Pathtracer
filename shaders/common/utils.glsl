#define FLT_MAX 3.402823466e+38
#define FLT_MIN 1.175494351e-38
#define PI 3.14159265359
#define TWO_PI 6.28318530718
#define PHI 1.61803398874

// --- Random ---
uvec4 seed;
ivec2 pixel;

void InitRNG(vec2 p, int frame)
{
    pixel = ivec2(p);
    seed = uvec4(p, uint(frame), uint(p.x) + uint(p.y));
}

void pcg4d(inout uvec4 v)
{
    v = v * 1664525u + 1013904223u;
    v.x += v.y * v.w;
    v.y += v.z * v.x;
    v.z += v.x * v.y;
    v.w += v.y * v.z;
    v = v ^ (v >> 16u);
    v.x += v.y * v.w;
    v.y += v.z * v.x;
    v.z += v.x * v.y;
    v.w += v.y * v.z;
}

float rand()
{
    pcg4d(seed);
    return float(seed.x) / float(0xffffffffu);
}
// --- Random ---

bool solveQuadratic(float a, float b, float c, inout float x0, inout float x1)
{
    float discr = b * b - 4 * a * c;
    if (discr < 0) return false;
    if (discr == 0)
        x0 = x1 = -0.5f * b / a;
    else
    {
        float q = (b > 0) ? -0.5f * (b + sqrt(discr)) : -0.5f * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }
    if (x0 > x1)
    {
        float temp = x0;
        x0 = x1;
        x1 = temp;
    }
    return true;
}

int floatToOrderedInt(float floatVal)
{
    int intVal = floatBitsToInt(floatVal);
    return (intVal >= 0) ? intVal : intVal ^ 0x7FFFFFFF;
}

float orderedIntToFloat(int intVal)
{
    return intBitsToFloat((intVal >= 0) ? intVal : intVal ^ 0x7FFFFFFF);
}

ivec3 floatToOrderedIntVec3(vec3 floatVal)
{
    return ivec3(floatToOrderedInt(floatVal.x), floatToOrderedInt(floatVal.y), floatToOrderedInt(floatVal.z));
}

vec3 orderedIntToFloatVec3(ivec3 intVal)
{
    return vec3(orderedIntToFloat(intVal.x), orderedIntToFloat(intVal.y), orderedIntToFloat(intVal.z));
}

void onb(in vec3 N, inout vec3 T, inout vec3 B)
{
    vec3 up = abs(N.z) < 0.9999999 ? vec3(0, 0, 1) : vec3(1, 0, 0);
    T = normalize(cross(up, N));
    B = cross(N, T);
}

vec3 sampleHemisphereUniform(float r1, float r2)
{
    float r = sqrt(1.0 - r1 * r1);
    float phi = TWO_PI * r2;

    float x = r * cos(phi);
    float y = r * sin(phi);
    float z = r1;
    return vec3(x, y, z);
}

vec3 sampleHemisphereCosine(float r1, float r2)
{
    float r = sqrt(r1);
    float phi = TWO_PI * r2;

    float x = r * cos(phi);
    float y = r * sin(phi);
    float z = sqrt(1.0 - r1);
    return vec3(x, y, z);
}

vec3 worldToTangent(vec3 dir, vec3 N)
{
    vec3 T, B;
    onb(N, T, B);
    return dir.x * T + dir.y * B + dir.z * N;
}
