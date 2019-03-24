#ifndef UTILS_H
#define UTILS_H

typedef struct {
    unsigned char b;
    unsigned char g;
    unsigned char r;
} pixel;

typedef struct {
    float x;
    float y;
    float z;
} vertex3;

void vec_sum(float *v0, float *v1, float *res);
void vec_diff(float *v0, float *v1, float *res);
void vec_cross(float *v0, float *v1, float *res);
float vec_dot(float *v0, float *v1);
float vec_len(float *v0);
void vec_normalized(float *v0, float *res);
void matrix_44_mul(float m0[4][4], float m1[4][4], float res[4][4]);
void vec4_matrix4_mul(float v[4], float m[4][4], int res[3]);

#endif