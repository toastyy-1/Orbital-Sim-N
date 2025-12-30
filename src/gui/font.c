#include "font.h"
#include "GL_renderer.h"
#include <stdio.h>
#include <stdlib.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define ATLAS 512
#define MAX_CHARS 4096

static stbtt_bakedchar cdata[96];

font_t initFont(const char* path, float size) {
    font_t f = {0};

    FILE* fp = fopen(path, "rb");
    if (!fp) return f;
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    rewind(fp);
    unsigned char* buf = malloc(len);
    fread(buf, 1, len, fp);
    fclose(fp);

    unsigned char* bmp = malloc(ATLAS * ATLAS);
    stbtt_BakeFontBitmap(buf, 0, size, bmp, ATLAS, ATLAS, 32, 96, cdata);
    free(buf);

    glGenTextures(1, &f.tex);
    glBindTexture(GL_TEXTURE_2D, f.tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ATLAS, ATLAS, 0, GL_RED, GL_UNSIGNED_BYTE, bmp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(bmp);

    f.shader = createShaderProgram("shaders/text.vert", "shaders/text.frag");
    f.verts = malloc(MAX_CHARS * 24 * sizeof(float));

    glGenVertexArrays(1, &f.vao);
    glGenBuffers(1, &f.vbo);
    glBindVertexArray(f.vao);
    glBindBuffer(GL_ARRAY_BUFFER, f.vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_CHARS * 24 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    return f;
}

void addText(font_t* font, float x, float y, const char* text, float scale) {
    for (; *text; text++) {
        int c = *text - 32;
        if (c < 0 || c >= 96) continue;
        stbtt_bakedchar* b = &cdata[c];

        float x0 = x + b->xoff * scale, y0 = y + b->yoff * scale;
        float x1 = x0 + (float)(b->x1 - b->x0) * scale, y1 = y0 + (float)(b->y1 - b->y0) * scale;
        float s0 = b->x0 / (float)ATLAS, t0 = b->y0 / (float)ATLAS;
        float s1 = b->x1 / (float)ATLAS, t1 = b->y1 / (float)ATLAS;

        float* v = &font->verts[font->count++ * 24];
        v[0]=x0; v[1]=y0; v[2]=s0; v[3]=t0;
        v[4]=x1; v[5]=y0; v[6]=s1; v[7]=t0;
        v[8]=x1; v[9]=y1; v[10]=s1; v[11]=t1;
        v[12]=x0; v[13]=y0; v[14]=s0; v[15]=t0;
        v[16]=x1; v[17]=y1; v[18]=s1; v[19]=t1;
        v[20]=x0; v[21]=y1; v[22]=s0; v[23]=t1;

        x += b->xadvance * scale;
    }
}

void renderText(font_t* font, float window_w, float window_h, float r, float g, float b) {
    if (!font->count) return;

    glDisable(GL_DEPTH_TEST);
    glUseProgram(font->shader);

    float proj[16] =   {2/window_w,0,0,0,
                        0,-2/window_h,0,0,
                        0,0,-1,0,
                        -1,1,0,1};
    glUniformMatrix4fv(glGetUniformLocation(font->shader, "proj"), 1, GL_FALSE, proj);
    glUniform3f(glGetUniformLocation(font->shader, "color"), r, g, b);

    glBindTexture(GL_TEXTURE_2D, font->tex);
    glBindBuffer(GL_ARRAY_BUFFER, font->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, font->count * 24 * sizeof(float), font->verts);
    glBindVertexArray(font->vao);
    glDrawArrays(GL_TRIANGLES, 0, font->count * 6);

    glEnable(GL_DEPTH_TEST);
    font->count = 0;
}

void freeFont(font_t* font) {
    free(font->verts);
    glDeleteTextures(1, &font->tex);
    glDeleteProgram(font->shader);
    glDeleteVertexArrays(1, &font->vao);
    glDeleteBuffers(1, &font->vbo);
}