#ifndef _LEDS_H
#define _LEDS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "periph/gpio.h"

int init_leds(gpio_t, gpio_t, gpio_t);

void set_pin(gpio_t);
void clear_pin(gpio_t);

#endif