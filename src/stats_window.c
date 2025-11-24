#include "stats_window.h"
#include "config.h"
#include "sdl_elements.h"
#include "sim_calculations.h"
#include <stdio.h>

extern SDL_Color TEXT_COLOR;

void renderGraph(SDL_Renderer* renderer, body_properties_t* bodies, int num_bodies, window_params_t wp) {

}

// the stats box that shows stats yay
double initial_total_energy;
bool measured_initial_total_energy = false;
void renderStatsBox(SDL_Renderer* renderer, body_properties_t* bodies, int num_bodies, spacecraft_properties_t* sc, int num_craft, window_params_t wp) {
    int margin_x    = (int)(wp.window_size_x * 0.02f);
    int top_y       = (int)(wp.window_size_y * 0.07f);
    int line_height = (int)(wp.window_size_y * 0.02f);

    if (!bodies) return;

    int y = top_y;

    // velocities
    for (int i = 0; i < num_bodies; i++) {
        char vel_text[32];
        snprintf(vel_text, sizeof(vel_text), "Vel of %s: %.2e", bodies[i].name, bodies[i].vel);
        SDL_WriteText(renderer, g_font, vel_text, margin_x, y, TEXT_COLOR);
        y += line_height;
    }

    // kinetic energies
    for (int i = 0; i < num_bodies; i++) {
        char ke_text[32];
        snprintf(ke_text, sizeof(ke_text), "KE of %s: %.2e", bodies[i].name, bodies[i].kinetic_energy);
        SDL_WriteText(renderer, g_font, ke_text, margin_x, y, TEXT_COLOR);
        y += line_height;
    }

    // total system energy
    double total_energy = calculateTotalSystemEnergy(bodies, sc, num_bodies, num_craft);
    char total_energy_text[32];
    snprintf(total_energy_text, sizeof(total_energy_text), "Total System Energy: %.2e", total_energy);
    SDL_WriteText(renderer, g_font, total_energy_text, margin_x, y, TEXT_COLOR);
    y += line_height;

    // total error
    if (!measured_initial_total_energy) {
        initial_total_energy = total_energy;
        measured_initial_total_energy = true;
    }
    double error = (initial_total_energy != 0) ? (initial_total_energy - total_energy) / initial_total_energy * 100.0 : 0.0;
    char error_text[32];
    snprintf(error_text, sizeof(error_text), "Energy Error: %.4f%%", error);
    SDL_WriteText(renderer, g_font, error_text, margin_x, y, TEXT_COLOR);

    renderGraph(renderer, bodies, num_bodies, wp);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// SDL WINDOW STUFF
////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize the stats window
bool statsWindowInit(stats_window_t* stats) {
    if (!stats) return false;

    stats->window = SDL_CreateWindow("Stats Window", 400, 300, SDL_WINDOW_RESIZABLE);
    if (!stats->window) {
        SDL_Log("Failed to create stats window: %s", SDL_GetError());
        return false;
    }

    stats->renderer = SDL_CreateRenderer(stats->window, NULL);
    if (!stats->renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(stats->window);
        return false;
    }

    stats->window_ID = SDL_GetWindowID(stats->window);
    stats->is_shown = true;
    return true;
}

// handle events for the stats window
void StatsWindow_handleEvent(stats_window_t* stats, SDL_Event* e) {
    // check if event belongs to this window
    if (e->type >= SDL_EVENT_WINDOW_FIRST && 
        e->type <= SDL_EVENT_WINDOW_LAST &&
        e->window.windowID == stats->window_ID) {
        
        if (e->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
            SDL_HideWindow(stats->window);
            stats->is_shown = false;
        }
    }
}

// show the window
void StatsWindow_show(stats_window_t* stats) {
    SDL_ShowWindow(stats->window);
    stats->is_shown = true;
}

// cleanup
void StatsWindow_destroy(stats_window_t* stats) {
    if (stats->renderer) {
        SDL_DestroyRenderer(stats->renderer);
    }
    if (stats->window) {
        SDL_DestroyWindow(stats->window);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN RENDER LOGIC
////////////////////////////////////////////////////////////////////////////////////////////////////
// render the stats window with data from main window
void StatsWindow_render(stats_window_t* stats, int fps, float posX, float posY, body_properties_t* gb, int num_bodies, spacecraft_properties_t* sc, int num_craft, window_params_t wp) {
    if (!stats->is_shown) return;

    SDL_SetRenderDrawColor(stats->renderer, 30, 30, 30, 255);
    SDL_RenderClear(stats->renderer);

    // draw the stats
    renderStatsBox(stats->renderer, gb, num_bodies, sc, num_craft, wp);

    SDL_RenderPresent(stats->renderer);
}