#ifndef _CORE_H
#define _CORE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

struct env {
    int id;
    int16_t temperature;
    // int16_t humidity;
    uint32_t pressure;
    time_t last_update;
};
typedef struct env env_t;

struct tank {
    int id;
    int ph;
    time_t last_update;
};
typedef struct tank tank_t;

enum state {
    OK,
    SOMETHING_WRONG,
    PANIC
};
typedef enum state state_e;

#endif