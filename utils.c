#include "utils.h"
#include <math.h>

void vec_sum(float *v0, float *v1, float *res)
{
    res[0] = v0[0] + v1[0];
    res[1] = v0[1] + v1[1];
    res[2] = v0[2] + v1[2];
}

void vec_diff(float *v0, float *v1, float *res)
{
    res[0] = v0[0] - v1[0];
    res[1] = v0[1] - v1[1];
    res[2] = v0[2] - v1[2];
}

void vec_cross(float *v0, float *v1, float *res)
{
    res[0] = v0[1] * v1[2] - v0[2] * v1[1];
    res[1] = v0[2] * v1[0] - v0[0] * v1[2];
    res[2] = v0[0] * v1[1] - v0[1] * v1[0];
}

float vec_len(float *v)
{
    return sqrt(pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2));
}

void vec_normalized(float *v, float *res)
{
    float l = vec_len(v);
    res[0] = v[0] / l;
    res[1] = v[1] / l;
    res[2] = v[2] / l;
}

float vec_dot(float *v0, float *v1)
{
    return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
}

void matrix_44_mul(float m0[4][4], float m1[4][4], float res[4][4])
{
    res[0][0] = m0[0][0] * m1[0][0] + m0[0][1] * m1[1][0] + m0[0][2] * m1[2][0] + m0[0][3] * m1[3][0];
    res[0][1] = m0[0][0] * m1[0][1] + m0[0][1] * m1[1][1] + m0[0][2] * m1[2][1] + m0[0][3] * m1[3][1];
    res[0][2] = m0[0][0] * m1[0][2] + m0[0][1] * m1[1][2] + m0[0][2] * m1[2][2] + m0[0][3] * m1[3][2];
    res[0][3] = m0[0][0] * m1[0][3] + m0[0][1] * m1[1][3] + m0[0][2] * m1[2][3] + m0[0][3] * m1[3][3];

    res[1][0] = m0[1][0] * m1[0][0] + m0[1][1] * m1[1][0] + m0[1][2] * m1[2][0] + m0[1][3] * m1[3][0];
    res[1][1] = m0[1][0] * m1[0][1] + m0[1][1] * m1[1][1] + m0[1][2] * m1[2][1] + m0[1][3] * m1[3][1];
    res[1][2] = m0[1][0] * m1[0][2] + m0[1][1] * m1[1][2] + m0[1][2] * m1[2][2] + m0[1][3] * m1[3][2];
    res[1][3] = m0[1][0] * m1[0][3] + m0[1][1] * m1[1][3] + m0[1][2] * m1[2][3] + m0[1][3] * m1[3][3];

    res[2][0] = m0[2][0] * m1[0][0] + m0[2][1] * m1[1][0] + m0[2][2] * m1[2][0] + m0[2][3] * m1[3][0];
    res[2][1] = m0[2][0] * m1[0][1] + m0[2][1] * m1[1][1] + m0[2][2] * m1[2][1] + m0[2][3] * m1[3][1];
    res[2][2] = m0[2][0] * m1[0][2] + m0[2][1] * m1[1][2] + m0[2][2] * m1[2][2] + m0[2][3] * m1[3][2];
    res[2][3] = m0[2][0] * m1[0][3] + m0[2][1] * m1[1][3] + m0[2][2] * m1[2][3] + m0[2][3] * m1[3][3];

    res[3][0] = m0[3][0] * m1[0][0] + m0[3][1] * m1[1][0] + m0[3][2] * m1[2][0] + m0[3][3] * m1[3][0];
    res[3][1] = m0[3][0] * m1[0][1] + m0[3][1] * m1[1][1] + m0[3][2] * m1[2][1] + m0[3][3] * m1[3][1];
    res[3][2] = m0[3][0] * m1[0][2] + m0[3][1] * m1[1][2] + m0[3][2] * m1[2][2] + m0[3][3] * m1[3][2];
    res[3][3] = m0[3][0] * m1[0][3] + m0[3][1] * m1[1][3] + m0[3][2] * m1[2][3] + m0[3][3] * m1[3][3];
}

void vec4_matrix4_mul(float v[4], float m[4][4], int res[3])
{
    float w = m[3][0] * v[0] + m[3][1] * v[1] + m[3][2] * v[2] + m[3][3];
    res[0] = (int)((m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3]) / w);
    res[1] = (int)((m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3]) / w);
    res[2] = (int)((m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3]) / w);
}