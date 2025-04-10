#define FLT_MAX 3.402823466e+38
#define FLT_MIN -3.402823466e+38
#define EPSILON 1e-10
#define PI 3.14159265359
#define TWO_PI 6.28318530718
#define DEG_TO_RAD 0.0174532925199432957
#define RAD_TO_DEG 57.295779513082320876

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
float minv3(vec3 v)
{
    return min(min(v.x, v.y), v.z);
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
float clampMin(float x, float minVal)
{
    return max(x, minVal);
}
float clampMax(float x, float maxVal)
{
    return min(x, maxVal);
}

vec3 clamp0(vec3 x)
{
    return vec3(clamp0(x.x), clamp0(x.y), clamp0(x.z));
}
vec3 clamp1(vec3 x)
{
    return vec3(clamp1(x.x), clamp1(x.y), clamp1(x.z));
}
vec3 clamp01(vec3 x)
{
    return vec3(clamp01(x.x), clamp01(x.y), clamp01(x.z));
}
vec3 clampMin(vec3 x, float minVal)
{
    return vec3(clampMin(x.x, minVal), clampMin(x.y, minVal), clampMin(x.z, minVal));
}
vec3 clampMax(vec3 x, float maxVal)
{
    return vec3(clampMax(x.x, maxVal), clampMax(x.y, maxVal), clampMax(x.z, maxVal));
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

float average(vec4 v)
{
    return (v.x + v.y + v.z + v.w) * 0.25;
}

float perlin_hash(vec3 p)
{
    return fract(sin(dot(p, vec3(127.1, 311.7, 74.7))) * 43758.5453);
}
vec3 perlin_fade(vec3 t)
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}
float perlin(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);

    float n000 = perlin_hash(i + vec3(0, 0, 0));
    float n001 = perlin_hash(i + vec3(0, 0, 1));
    float n010 = perlin_hash(i + vec3(0, 1, 0));
    float n011 = perlin_hash(i + vec3(0, 1, 1));
    float n100 = perlin_hash(i + vec3(1, 0, 0));
    float n101 = perlin_hash(i + vec3(1, 0, 1));
    float n110 = perlin_hash(i + vec3(1, 1, 0));
    float n111 = perlin_hash(i + vec3(1, 1, 1));

    vec3 u = perlin_fade(f);

    return mix(
        mix(
            mix(n000, n100, u.x),
            mix(n010, n110, u.x), u.y),
        mix(
            mix(n001, n101, u.x),
            mix(n011, n111, u.x), u.y),
        u.z);
}

float windyTurbulence(vec3 p, int octaves)
{
    float sum = 0.0;
    float amp = 1.0;
    float freq = 1.0;
    for (int i = 0; i < octaves; ++i)
    {
        sum += abs(perlin(p * freq)) * amp;
        freq *= 2.0;
        amp *= 0.5;
    }
    return sum;
}
float windyBump(vec3 p, float scale)
{
    return windyTurbulence(p * scale, 6);
}
vec3 windyBumpNormal(vec3 p, vec3 normal, float scale, float strength)
{
    float h = windyBump(p, scale);
    float dx = windyBump(p + vec3(0.001, 0, 0), scale) - h;
    float dy = windyBump(p + vec3(0, 0.001, 0), scale) - h;
    float dz = windyBump(p + vec3(0, 0, 0.001), scale) - h;
    vec3 grad = vec3(dx, dy, dz);
    return normalize(normal + grad * strength);
}
