#ifndef _BMP280_H
#define _BMP280_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <mutex.h>

#include "periph/i2c.h"

#include "bmx280.h"
#include "bmx280_params.h"

#define MIN_TEMP 20.0
#define MAX_TEMP 30.0

uint16_t get_env_raw_temperature(bmx280_t*);
uint32_t get_env_raw_pressure(bmx280_t*);
// uint16_t get_env_raw_humidity(bmx280_t);


int init_bmp280(bmx280_t);
char* str_temperature(uint16_t);
char* raw_temperature(uint16_t);

char* str_pressure(uint32_t);
char* raw_pressure(uint32_t);

#endif