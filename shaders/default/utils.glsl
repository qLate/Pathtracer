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
