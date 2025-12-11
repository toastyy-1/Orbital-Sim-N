//
// Created by java on 12/10/2025.
//

#ifndef ORBITSIMULATION_CRAFT_VIEW_H
#define ORBITSIMULATION_CRAFT_VIEW_H

#include <SDL3/SDL.h>
#include "config.h"

void craft_DrawCraft(SDL_Renderer* renderer, window_params_t* wp);
void craft_RenderCraftView(SDL_Renderer* renderer, window_params_t* wp);

#endif //ORBITSIMULATION_CRAFT_VIEW_H