#define FLT_MAX 3.402823466e+38
#define FLT_MIN -3.402823466e+38
#define EPSILON 1e-10
#define PI 3.14159265359
#define TWO_PI 6.28318530718

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
int randInt(int min, int max)
{
    pcg4d(seed);
    return int(seed.x) % (max - min + 1) + min;
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

vec3 worldToTangent(vec3 dir, vec3 N)
{
    vec3 T, B;
    onb(N, T, B);
    return dir.x * T + dir.y * B + dir.z * N;
}

float maxv3(vec3 v)
{
    return max(max(v.x, v.y), v.z);
}

vec3 sampleHemisphereUniform(float r1, float r2)
{
    float r = sqrt(max(1.0 - r1 * r1, 0));
    float phi = TWO_PI * r2;

    float x = r * cos(phi);
    float y = r * sin(phi);
    float z = r1;
    return vec3(x, y, z);
}

vec3 sampleHemisphereCosine(float r1, float r2)
{
    float r = sqrt(max(r1, 0));
    float phi = TWO_PI * r2;

    float x = r * cos(phi);
    float y = r * sin(phi);
    float z = sqrt(max(1.0 - r1, 0));
    return vec3(x, y, z);
}

float clamp0(float x)
{
    return max(x, 0.0);
}
float clamp1(float x)
{
    return min(x, 1.0);
}
float clamp01(float x)
{
    return clamp(x, 0.0, 1.0);
}

float luminance(vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

vec3 linearToGamma(vec3 color)
{
    return pow(color, vec3(0.454545)); // 1 / 2.2
}
float linearToGamma(float value)
{
    return pow(value, 0.454545); // 1 / 2.2
}

vec3 sampleTriangleUniform(vec3 p0, vec3 p1, vec3 p2, float r1, float r2)
{
    float u = sqrt(r1);
    float v = r2;
    return (1.0 - u) * p0 + (u * (1.0 - v)) * p1 + (u * v) * p2;
}

float powerHeuristic(float pdf1, float pdf2)
{
    float a2 = pdf1 * pdf1;
    float b2 = pdf2 * pdf2;
    return a2 / (a2 + b2 + 0.0001);
}
float balancedHeuristic(float pdf1, float pdf2)
{
    return pdf1 / (pdf1 + pdf2 + 0.0001);
}