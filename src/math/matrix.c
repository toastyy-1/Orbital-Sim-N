//
// Created by java on 12/27/25.
//

#include "matrix.h"
#include <math.h>
#include "../types.h"

// creates an identity matrix
mat4 mat4_identity(void) {
    mat4 m = {.m = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    }};
    return m;
}

// creates a translation matrix
mat4 mat4_translation(float x, float y, float z) {
    mat4 m = {.m = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1
    }};
    return m;
}

// creates a scale matrix
mat4 mat4_scale(float sx, float sy, float sz) {
    mat4 m = {.m = {
        sx, 0,  0,  0,
        0,  sy, 0,  0,
        0,  0,  sz, 0,
        0,  0,  0,  1
    }};
    return m;
}

// creates a rotation matrix around the X axis
mat4 mat4_rotationX(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    mat4 m = {.m = {
        1, 0,  0, 0,
        0, c, -s, 0,
        0, s,  c, 0,
        0, 0,  0, 1
    }};
    return m;
}

// creates a rotation matrix around the Y axis
mat4 mat4_rotationY(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    mat4 m = {.m = {
         c, 0, s, 0,
         0, 1, 0, 0,
        -s, 0, c, 0,
         0, 0, 0, 1
    }};
    return m;
}

// creates a rotation matrix around the Z axis
mat4 mat4_rotationZ(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    mat4 m = {.m = {
        c, -s, 0, 0,
        s,  c, 0, 0,
        0,  0, 1, 0,
        0,  0, 0, 1
    }};
    return m;
}

// matrix multiplication
mat4 mat4_mul(mat4 a, mat4 b) {
    mat4 r;

    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            r.m[col*4 + row] =
                a.m[0*4 + row] * b.m[col*4 + 0] +
                a.m[1*4 + row] * b.m[col*4 + 1] +
                a.m[2*4 + row] * b.m[col*4 + 2] +
                a.m[3*4 + row] * b.m[col*4 + 3];
        }
    }
    return r;
}

// transform a point by a matrix (for rotating camera position)
coord_t mat4_transformPoint(mat4 m, coord_t point) {
    coord_t result;
    result.x = m.m[0] * point.x + m.m[4] * point.y + m.m[8]  * point.z + m.m[12];
    result.y = m.m[1] * point.x + m.m[5] * point.y + m.m[9]  * point.z + m.m[13];
    result.z = m.m[2] * point.x + m.m[6] * point.y + m.m[10] * point.z + m.m[14];
    return result;
}