#ifndef _SEN0161_H
#define _SEN0161_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "periph/adc.h"
#include "analog_util.h"

#define ADC_IN_USE                  ADC_LINE(0)
#define ADC_RES                     ADC_RES_12BIT

#define DELAY                       (3LU * US_PER_SEC) /* 3 s */

#define NEUTRAL_PH 7
#define MIN_PH 1.5
#define MAX_PH 13.5

int init_sen0161(void);
float get_ph_value(void);
char* str_pH(float);
char* raw_pH(float);

#endif