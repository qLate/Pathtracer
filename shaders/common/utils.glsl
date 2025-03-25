#define FLT_MAX 3.402823466e+38
#define FLT_MIN 1.175494351e-38
#define PI 3.14159265359
#define PHI 1.61803398874

float random(in vec2 xy, in float seed) {
    return fract(sin(distance(xy * PHI, xy) * seed) * xy.x);
}

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

vec3 randomDirInHemisphere(float r1, float r2)
{
    float cosTheta = r1;
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    float phi = 2.0 * PI * r2;
    float x = sinTheta * cos(phi);
    float z = sinTheta * sin(phi);
    return vec3(x, cosTheta, z);
}

vec3 worldToTangent(vec3 dir, vec3 normal)
{
    vec3 tangent = normalize(cross(normal, dir));
    vec3 bitangent = cross(normal, tangent);
    return vec3(dot(dir, tangent), dot(dir, bitangent), dot(dir, normal));
}
