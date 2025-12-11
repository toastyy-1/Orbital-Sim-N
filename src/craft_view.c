//
// Created by java on 12/10/2025.
//

#include "craft_view.h"
#include "sdl_elements.h"
#include "config.h"

SDL_Color color1 = {240, 240, 245, 255};

// draws the shape of the craft in the craft view
void craft_DrawCraft(SDL_Renderer* renderer, window_params_t* wp) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_WriteText(renderer, g_font, "Hello", 67, 67, color1);
}

// toggleable craft view window
void craft_RenderCraftView(SDL_Renderer* renderer, window_params_t* wp) {
    craft_DrawCraft(renderer, wp);
}
