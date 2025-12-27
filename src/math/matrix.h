//
// Created by java on 12/27/25.
//

#ifndef ORBITSIMULATION_MATRIX_H
#define ORBITSIMULATION_MATRIX_H

#include "../types.h"

mat4 mat4_identity(void);
mat4 mat4_translation(float x, float y, float z);
mat4 mat4_scale(float sx, float sy, float sz);
mat4 mat4_rotationX(float angle);
mat4 mat4_rotationY(float angle);
mat4 mat4_rotationZ(float angle);
mat4 mat4_mul(mat4 a, mat4 b);
coord_t mat4_transformPoint(mat4 m, coord_t point);

#endif //ORBITSIMULATION_MATRIX_H