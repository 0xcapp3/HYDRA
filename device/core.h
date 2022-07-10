#ifndef _CORE_H
#define _CORE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include <math.h>
#include <stdint.h>
#include <time.h>

#include "net/emcute.h"

#include "bmx280.h"
#include "u8g2.h"

#define DEFAULT_NUM_OF_CLUSTERS 1
#define DEFAULT_NUM_OF_TANKS 1

#define DEBUG

enum state {
    OK,
    SOMETHING_WRONG,
    PANIC
};
typedef enum state state_e;

#define DEFAULT_STATE OK

struct nutrient_solution {
    int id;
    // int16_t temperature;
    float ph;
    time_t last_update;
};
typedef struct nutrient_solution nutrient_solution_t;

struct environment {
    int id;
    bmx280_t* sensor;
    time_t last_update;
};
typedef struct environment environment_t;

struct tank {
    int id;
    environment_t env;
    nutrient_solution_t solution;
    u8g2_t display;
    state_e state;
    time_t last_update;
};
typedef struct tank tank_t;

struct cluster {
    int id;
    tank_t tanks[DEFAULT_NUM_OF_TANKS];
};
typedef struct cluster cluster_t;

state_e hydra_state = DEFAULT_STATE;
cluster_t hydra_clusters[DEFAULT_NUM_OF_CLUSTERS];

#endif