#ifndef GLOBALS_H
#define GLOBALS_H

#include <pthread.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

extern TTF_Font* g_font;
extern TTF_Font* g_font_small;
extern pthread_mutex_t sim_vars_mutex;
extern const double G;
extern char* SIMULATION_FILENAME;

extern SDL_Color TEXT_COLOR;
extern SDL_Color BUTTON_COLOR;
extern SDL_Color BUTTON_HOVER_COLOR;
extern SDL_Color ACCENT_COLOR;

#endif
