#ifndef FONT_H
#define FONT_H

#include <GL/glew.h>

typedef struct {
    GLuint tex, shader, vao, vbo;
    float* verts;
    int count;
} font_t;

font_t initFont(const char* path, float size);
void addText(font_t* font, float x, float y, const char* text, float scale);
void renderText(font_t* font, float window_w, float window_h, float r, float g, float b);
void freeFont(font_t* font);

#endif