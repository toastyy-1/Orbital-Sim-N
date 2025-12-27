//
// Created by java on 12/25/25.
//

#include "GL_renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../globals.h"

char* loadShaderSource(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "Failed to open shader file: %s\n", filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* source = (char*)malloc(size + 1);

    fread(source, 1, size, file);
    source[size] = '\0';

    fclose(file);
    return source;
}

GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    // load shader sources from files
    char* vertexShaderSource = loadShaderSource(vertexPath);
    char* fragmentShaderSource = loadShaderSource(fragmentPath);

    // compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const char**)&vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const char**)&fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // link the shaders into a program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // free the shader source strings (byebye)
    free(vertexShaderSource);
    free(fragmentShaderSource);

    return shaderProgram;
}

VBO_t createVBO(const float* vertices, const size_t vertexDataSize) {
    VBO_t vbo = {0};

    glGenVertexArrays(1, &vbo.VAO);
    glGenBuffers(1, &vbo.VBO);

    glBindVertexArray(vbo.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo.VBO);
    glBufferData(GL_ARRAY_BUFFER, (long)vertexDataSize, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // unbind
    glBindVertexArray(0);

    return vbo;
}

void deleteVBO(const VBO_t vbo) {
    glDeleteVertexArrays(1, &vbo.VAO);
    glDeleteBuffers(1, &vbo.VBO);
}

// normalize a 3D vector
static void normalize_3d(float v[3]) {
    float length = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (length > 0.0f) {
        v[0] /= length;
        v[1] /= length;
        v[2] /= length;
    }
}

// cross product of two 3D vectors
static void cross_product_3d(const float a[3], const float b[3], float result[3]) {
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
}

// creates a view matrix that looks at the origin from cameraPos
mat4 createViewMatrix_originCentered(const float cameraPos[3]) {
    mat4 viewMatrix;
    float forward[3] = {-cameraPos[0], -cameraPos[1], -cameraPos[2]};
    normalize_3d(forward);
    float up[3] = {0.0f, 1.0f, 0.0f};
    float right[3];
    cross_product_3d(forward, up, right);
    normalize_3d(right);
    cross_product_3d(right, forward, up);

    // build view matrix
    viewMatrix.m[0] = right[0];
    viewMatrix.m[1] = up[0];
    viewMatrix.m[2] = -forward[0];
    viewMatrix.m[3] = 0.0f;
    viewMatrix.m[4] = right[1];
    viewMatrix.m[5] = up[1];
    viewMatrix.m[6] = -forward[1];
    viewMatrix.m[7] = 0.0f;
    viewMatrix.m[8] = right[2];
    viewMatrix.m[9] = up[2];
    viewMatrix.m[10] = -forward[2];
    viewMatrix.m[11] = 0.0f;
    viewMatrix.m[12] = -(right[0] * cameraPos[0] + right[1] * cameraPos[1] + right[2] * cameraPos[2]);
    viewMatrix.m[13] = -(up[0] * cameraPos[0] + up[1] * cameraPos[1] + up[2] * cameraPos[2]);
    viewMatrix.m[14] = (forward[0] * cameraPos[0] + forward[1] * cameraPos[1] + forward[2] * cameraPos[2]);
    viewMatrix.m[15] = 1.0f;
    return viewMatrix;
}

// creates a perspective projection matrix
mat4 createProjectionMatrix(const float fov, const float aspect, const float near, const float far) {
    mat4 projMatrix;
    float f = 1.0f / tanf(fov * 0.5f);

    projMatrix.m[0] = f / aspect;
    projMatrix.m[1] = 0.0f;
    projMatrix.m[2] = 0.0f;
    projMatrix.m[3] = 0.0f;

    projMatrix.m[4] = 0.0f;
    projMatrix.m[5] = f;
    projMatrix.m[6] = 0.0f;
    projMatrix.m[7] = 0.0f;

    projMatrix.m[8] = 0.0f;
    projMatrix.m[9] = 0.0f;
    projMatrix.m[10] = (far + near) / (near - far);
    projMatrix.m[11] = -1.0f;

    projMatrix.m[12] = 0.0f;
    projMatrix.m[13] = 0.0f;
    projMatrix.m[14] = (2.0f * far * near) / (near - far);
    projMatrix.m[15] = 0.0f;
    return projMatrix;
}

// sets a matrix uniform in the shader
void setMatrixUniform(const GLuint shaderProgram, const char* name, const mat4* matrix) {
    GLint location = glGetUniformLocation(shaderProgram, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix->m);
}

// main function for handling the camera, should be called in main().
void castCamera(const sim_properties_t sim, const GLuint shaderProgram) {
    // apply zoom to camera position unit vector
    float zoomedCameraPos[3] = {
        sim.wp.camera_pos.x * sim.wp.zoom,
        sim.wp.camera_pos.y * sim.wp.zoom,
        sim.wp.camera_pos.z * sim.wp.zoom
    };

    // create view matrix
    mat4 viewMatrix = createViewMatrix_originCentered(zoomedCameraPos);

    // create projection matrix
    float aspect = sim.wp.window_size_x / sim.wp.window_size_y;
    mat4 projMatrix = createProjectionMatrix(3.14159f / 4.0f, aspect, 0.1f, 100000.0f);

    // set matrices in shader
    setMatrixUniform(shaderProgram, "view", &viewMatrix);
    setMatrixUniform(shaderProgram, "projection", &projMatrix);
}

// sphere mesh generation
sphere_mesh_t generateUnitSphere(unsigned int stacks, unsigned int sectors) {
    sphere_mesh_t sphere = {0};

    // each quad (stack-sector intersection) creates 2 triangles = 6 vertices
    // total quads = stacks * sectors
    sphere.vertex_count = stacks * sectors * 6;
    sphere.data_size = sphere.vertex_count * 6 * sizeof(float); // 6 floats per vertex (pos + normal)

    sphere.vertices = (float*)malloc(sphere.data_size);
    if (!sphere.vertices) {
        fprintf(stderr, "Failed to allocate memory for sphere vertices\n");
        return sphere;
    }

    float* data = sphere.vertices;
    const float PI = 3.14159265359f;

    // generate vertices for each stack and sector
    for (unsigned int i = 0; i < stacks; ++i) {
        float theta1 = (float)i * PI / (float)stacks;
        float theta2 = (float)(i + 1) * PI / (float)stacks;

        for (unsigned int j = 0; j < sectors; ++j) {
            float phi1 = (float)j * 2.0f * PI / (float)sectors;
            float phi2 = (float)(j + 1) * 2.0f * PI / (float)sectors;

            // calculate 4 vertices of the quad
            // v1 (top-left)
            float v1_x = cosf(phi1) * sinf(theta1);
            float v1_y = cosf(theta1);
            float v1_z = sinf(phi1) * sinf(theta1);

            // v2 (bottom-left)
            float v2_x = cosf(phi1) * sinf(theta2);
            float v2_y = cosf(theta2);
            float v2_z = sinf(phi1) * sinf(theta2);

            // v3 (bottom-right)
            float v3_x = cosf(phi2) * sinf(theta2);
            float v3_y = cosf(theta2);
            float v3_z = sinf(phi2) * sinf(theta2);

            // v4 (top-right)
            float v4_x = cosf(phi2) * sinf(theta1);
            float v4_y = cosf(theta1);
            float v4_z = sinf(phi2) * sinf(theta1);

            // first triangle (v1, v2, v3)
            // v1
            *data++ = v1_x; *data++ = v1_y; *data++ = v1_z; // position
            *data++ = v1_x; *data++ = v1_y; *data++ = v1_z; // normal (same as position for unit sphere)
            // v2
            *data++ = v2_x; *data++ = v2_y; *data++ = v2_z;
            *data++ = v2_x; *data++ = v2_y; *data++ = v2_z;
            // v3
            *data++ = v3_x; *data++ = v3_y; *data++ = v3_z;
            *data++ = v3_x; *data++ = v3_y; *data++ = v3_z;

            // second triangle (v1, v3, v4)
            // v1
            *data++ = v1_x; *data++ = v1_y; *data++ = v1_z;
            *data++ = v1_x; *data++ = v1_y; *data++ = v1_z;
            // v3
            *data++ = v3_x; *data++ = v3_y; *data++ = v3_z;
            *data++ = v3_x; *data++ = v3_y; *data++ = v3_z;
            // v4
            *data++ = v4_x; *data++ = v4_y; *data++ = v4_z;
            *data++ = v4_x; *data++ = v4_y; *data++ = v4_z;
        }
    }

    return sphere;
}

void freeSphere(sphere_mesh_t* sphere) {
    if (sphere && sphere->vertices) {
        free(sphere->vertices);
        sphere->vertices = NULL;
        sphere->vertex_count = 0;
        sphere->data_size = 0;
    }
}
