#include "config.h"
#include "sim_calculations.h"
#include "sdl_elements.h"
#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

// json handling logic for reading json files
void readSimulationJSON(const char* FILENAME, body_properties_t* gb, spacecraft_properties_t* sc) {
    #ifdef _WIN32
    FILE *fp;
    fopen_s(&fp, FILENAME, "r");
    #else
    FILE *fp = fopen(FILENAME, "r");
    #endif
    if (fp == NULL) {
        displayError("ERROR", "Error: Could not open simulation JSON file");
        return;
    }

    // read entire file into buffer
    fseek(fp, 0, SEEK_END);
    const long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* json_buffer = (char*)malloc(file_size + 1);
    if (json_buffer == NULL) {
        displayError("ERROR", "Error: Could not allocate memory for JSON buffer");
        fclose(fp);
        return;
    }

    fread(json_buffer, 1, file_size, fp);
    json_buffer[file_size] = '\0';
    fclose(fp);

    // parse json
    cJSON* json = cJSON_Parse(json_buffer);
    free(json_buffer);

    if (json == NULL) {
        displayError("ERROR", "Error: Failed to parse simulation JSON");
        return;
    }

    // get bodies array
    const cJSON* bodies = cJSON_GetObjectItemCaseSensitive(json, "bodies");
    if (bodies != NULL && cJSON_IsArray(bodies)) {
        const cJSON* body = NULL;
        cJSON_ArrayForEach(body, bodies) {
            cJSON* name_item = cJSON_GetObjectItemCaseSensitive(body, "name");
            cJSON* mass_item = cJSON_GetObjectItemCaseSensitive(body, "mass");
            cJSON* radius_item = cJSON_GetObjectItemCaseSensitive(body, "radius");
            cJSON* pos_x_item = cJSON_GetObjectItemCaseSensitive(body, "pos_x");
            cJSON* pos_y_item = cJSON_GetObjectItemCaseSensitive(body, "pos_y");
            cJSON* vel_x_item = cJSON_GetObjectItemCaseSensitive(body, "vel_x");
            cJSON* vel_y_item = cJSON_GetObjectItemCaseSensitive(body, "vel_y");

            body_addOrbitalBody(gb,
                                name_item->valuestring,
                                mass_item->valuedouble,
                                radius_item->valuedouble,
                                pos_x_item->valuedouble,
                                pos_y_item->valuedouble,
                                vel_x_item->valuedouble,
                                vel_y_item->valuedouble);
        }
    }

    // get spacecraft array
    const cJSON* spacecraft = cJSON_GetObjectItemCaseSensitive(json, "spacecraft");
    if (spacecraft != NULL && cJSON_IsArray(spacecraft)) {
        cJSON* craft = NULL;
        cJSON_ArrayForEach(craft, spacecraft) {
            cJSON* name_item = cJSON_GetObjectItemCaseSensitive(craft, "name");
            cJSON* pos_x_item = cJSON_GetObjectItemCaseSensitive(craft, "pos_x");
            cJSON* pos_y_item = cJSON_GetObjectItemCaseSensitive(craft, "pos_y");
            cJSON* vel_x_item = cJSON_GetObjectItemCaseSensitive(craft, "vel_x");
            cJSON* vel_y_item = cJSON_GetObjectItemCaseSensitive(craft, "vel_y");
            cJSON* dry_mass_item = cJSON_GetObjectItemCaseSensitive(craft, "dry_mass");
            cJSON* fuel_mass_item = cJSON_GetObjectItemCaseSensitive(craft, "fuel_mass");
            cJSON* thrust_item = cJSON_GetObjectItemCaseSensitive(craft, "thrust");
            cJSON* specific_impulse_item = cJSON_GetObjectItemCaseSensitive(craft, "specific_impulse");
            cJSON* mass_flow_rate_item = cJSON_GetObjectItemCaseSensitive(craft, "mass_flow_rate");
            cJSON* attitude_item = cJSON_GetObjectItemCaseSensitive(craft, "attitude");
            cJSON* moment_of_inertia_item = cJSON_GetObjectItemCaseSensitive(craft, "moment_of_inertia");
            cJSON* nozzle_gimbal_range_item = cJSON_GetObjectItemCaseSensitive(craft, "nozzle_gimbal_range");

            // parse burns array
            cJSON* burns_array = cJSON_GetObjectItemCaseSensitive(craft, "burns");
            int num_burns = 0;
            burn_properties_t* burns = NULL;

            if (burns_array != NULL && cJSON_IsArray(burns_array)) {
                num_burns = cJSON_GetArraySize(burns_array);
                if (num_burns > 0) {
                    burns = (burn_properties_t*)malloc(num_burns * sizeof(burn_properties_t));
                    cJSON* burn = NULL;
                    int burn_idx = 0;
                    cJSON_ArrayForEach(burn, burns_array) {
                        cJSON* start_time = cJSON_GetObjectItemCaseSensitive(burn, "start_time");
                        cJSON* duration = cJSON_GetObjectItemCaseSensitive(burn, "duration");
                        cJSON* heading = cJSON_GetObjectItemCaseSensitive(burn, "heading");
                        cJSON* throttle = cJSON_GetObjectItemCaseSensitive(burn, "throttle");

                        burns[burn_idx].burn_start_time = start_time->valuedouble;
                        burns[burn_idx].burn_end_time = start_time->valuedouble + duration->valuedouble;
                        burns[burn_idx].burn_heading = heading->valuedouble;
                        burns[burn_idx].throttle = throttle->valuedouble;
                        burn_idx++;
                    }
                }
            }

            craft_addSpacecraft(sc,
                                name_item->valuestring,
                                pos_x_item->valuedouble,
                                pos_y_item->valuedouble,
                                vel_x_item->valuedouble,
                                vel_y_item->valuedouble,
                                dry_mass_item->valuedouble,
                                fuel_mass_item->valuedouble,
                                thrust_item->valuedouble,
                                specific_impulse_item->valuedouble,
                                mass_flow_rate_item->valuedouble,
                                attitude_item->valuedouble,
                                moment_of_inertia_item->valuedouble,
                                nozzle_gimbal_range_item->valuedouble,
                                burns, num_burns);

            // free burns array after adding spacecraft
            if (burns != NULL) {
                free(burns);
            }
        }
    }

    cJSON_Delete(json);
}


// cleanup for main
void cleanup(void* args) {
    const cleanup_args* c = (cleanup_args*)args;

    // free all bodies
    for (int i = 0; i < c->gb->count; i++) {
        free(c->gb->names[i]);
    }

    // free cache arrays for each body
    if (c->gb->cached_body_coords_x != NULL) {
        for (int i = 0; i < c->gb->count; i++) {
            free(c->gb->cached_body_coords_x[i]);
        }
        free(c->gb->cached_body_coords_x);
    }
    if (c->gb->cached_body_coords_y != NULL) {
        for (int i = 0; i < c->gb->count; i++) {
            free(c->gb->cached_body_coords_y[i]);
        }
        free(c->gb->cached_body_coords_y);
    }

    free(c->gb->names);
    free(c->gb->mass);
    free(c->gb->radius);
    free(c->gb->pixel_radius);
    free(c->gb->pos_x);
    free(c->gb->pos_y);
    free(c->gb->pixel_coordinates_x);
    free(c->gb->pixel_coordinates_y);
    free(c->gb->vel_x);
    free(c->gb->vel_y);
    free(c->gb->vel);
    free(c->gb->acc_x);
    free(c->gb->acc_y);
    free(c->gb->acc_x_prev);
    free(c->gb->acc_y_prev);
    free(c->gb->force_x);
    free(c->gb->force_y);
    free(c->gb->kinetic_energy);

    // free all spacecraft
    for (int i = 0; i < c->sc->count; i++) {
        free(c->sc->names[i]);
        free(c->sc->burn_properties[i]);
    }
    free(c->sc->names);
    free(c->sc->current_total_mass);
    free(c->sc->dry_mass);
    free(c->sc->fuel_mass);
    free(c->sc->pos_x);
    free(c->sc->pos_y);
    free(c->sc->pixel_coordinates_x);
    free(c->sc->pixel_coordinates_y);
    free(c->sc->attitude);
    free(c->sc->vel_x);
    free(c->sc->vel_y);
    free(c->sc->vel);
    free(c->sc->rotational_v);
    free(c->sc->momentum);
    free(c->sc->acc_x);
    free(c->sc->acc_y);
    free(c->sc->acc_x_prev);
    free(c->sc->acc_y_prev);
    free(c->sc->rotational_a);
    free(c->sc->moment_of_inertia);
    free(c->sc->grav_force_x);
    free(c->sc->grav_force_y);
    free(c->sc->torque);
    free(c->sc->thrust);
    free(c->sc->mass_flow_rate);
    free(c->sc->specific_impulse);
    free(c->sc->throttle);
    free(c->sc->nozzle_gimbal_range);
    free(c->sc->nozzle_velocity);
    free(c->sc->engine_on);
    free(c->sc->num_burns);
    free(c->sc->burn_properties);
}