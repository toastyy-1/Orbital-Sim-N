//
// Created by java on 12/11/2025.
//
#include "../globals.h"
#include "../types.h"
#include "craft_view.h"
#include <math.h>
#include <stdio.h>

#include "renderer.h"

// draws the shape of the craft in the craft view
void craft_DrawCraft(SDL_Renderer* renderer, const window_params_t* wp, const spacecraft_properties_t* sc) {
    if (sc != NULL && sc->count > 0 && sc->attitude != NULL) {
        const float center_x = wp->window_size_x / 2.0f;
        const float center_y = wp->window_size_y / 2.0f;

        // arrow shape
        const float arrow_length = 40.0f;
        const float arrow_width = 25.0f;
        const float vertices[3][2] = {
            {0.0f, -arrow_length / 2.0f},
            {-arrow_width / 2.0f, arrow_length / 2.0f},
            {arrow_width / 2.0f, arrow_length / 2.0f}
        };

        // rotate and translate vertices based on heading (radians -- ccw positive)
        SDL_FPoint points[4];
        for (int i = 0; i < 3; i++) {
            const float x = vertices[i][0];
            const float y = vertices[i][1];

            const float rotated_x = x * cosf((float)sc->attitude[0]) - y * sinf((float)sc->attitude[0]);
            const float rotated_y = x * sinf((float)sc->attitude[0]) + y * cosf((float)sc->attitude[0]);

            points[i].x = center_x + rotated_x;
            points[i].y = center_y + rotated_y;
        }
        points[3] = points[0];

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Vertex triangle_vertices[3];
        for (int i = 0; i < 3; i++) {
            triangle_vertices[i].position = points[i];
            triangle_vertices[i].color.r = 255;
            triangle_vertices[i].color.g = 255;
            triangle_vertices[i].color.b = 255;
            triangle_vertices[i].color.a = 255;
        }

        const int indices[3] = {0, 1, 2};
        SDL_RenderGeometry(renderer, NULL, triangle_vertices, 3, indices, 3);
    }
}

// draws the arrows that point to planets and their distances relative to the craft
void craft_DrawRelativeArrows(SDL_Renderer* renderer, const window_params_t* wp, const body_properties_t* gb, const spacecraft_properties_t* sc) {
    if (sc != NULL && sc->count > 0 && gb != NULL && gb->count > 0) {
        const float center_x = wp->window_size_x / 2.0f;
        const float center_y = wp->window_size_y / 2.0f;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        const float arrow_length = 200.0f; // fixed arrow length

        for (int i = 0; i < gb->count; i++) {
            // calculate relative position to the craft
            const float relative_x = (float)(gb->pos_x[i] - sc->pos_x[0]);
            const float relative_y = (float)(gb->pos_y[i] - sc->pos_y[0]);

            char dist_text[32];
            const float distance = (sqrtf(relative_x * relative_x + relative_y * relative_y) - (float)gb->radius[i]) / 1000.0f;
            snprintf(dist_text, sizeof(dist_text), "%.1f km", distance);

            // calculate heading to the body
            const float relative_body_heading = atan2f(-relative_y, relative_x);

            // create a fixed-length arrow pointing in the direction of the body
            const float pointer_ending_x = center_x + arrow_length * cosf(relative_body_heading);
            const float pointer_ending_y = center_y + arrow_length * sinf(relative_body_heading);

            SDL_RenderLine(renderer, center_x, center_y, pointer_ending_x, pointer_ending_y);
            SDL_WriteText(renderer, g_font_small, gb->names[i], pointer_ending_x, pointer_ending_y, TEXT_COLOR);
            SDL_WriteText(renderer, g_font_small, dist_text, pointer_ending_x, pointer_ending_y + 15, TEXT_COLOR);
        }
    }
}

// toggleable craft view window
void craft_RenderCraftView(SDL_Renderer* renderer, window_params_t* wp, body_properties_t* gb, spacecraft_properties_t* sc) {
    craft_DrawCraft(renderer, wp, sc);
    craft_DrawRelativeArrows(renderer, wp, gb, sc);
}
