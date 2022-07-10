#include "leds.h"

int init_leds(gpio_t red, gpio_t yellow, gpio_t green) {
    printf("[+] Initializing semaphore leds...\r\n");
    // gpio_t pin_out_r = GPIO_PIN(PORT_B, 4);
    red = GPIO_PIN(PORT_B, 4);
    if (gpio_init(red, GPIO_OUT)) {
        printf("[-] Error to initialize GPIO_PIN(%d %d) (aka red led)\r\n", PORT_B, 4);
        return -1;
    }

    // gpio_t pin_out_y = GPIO_PIN(PORT_B, 5);
    yellow = GPIO_PIN(PORT_B, 5);
    if (gpio_init(yellow, GPIO_OUT)) {
        printf("[-] Error to initialize GPIO_PIN(%d %d) (aka yellow led)\r\n", PORT_B, 5);
        return -1;
    }

    // gpio_t pin_out_g = GPIO_PIN(PORT_B, 3);
    green = GPIO_PIN(PORT_B, 3);
    if (gpio_init(green, GPIO_OUT)) {
        printf("[-] Error to initialize GPIO_PIN(%d %d) (aka green led)\r\n", PORT_B, 3);
        return -1;
    }

    return 0;
}

void set_pin(gpio_t pin) {
    // printf("Set pin to HIGH\r\n");
    gpio_set(pin);
}

void clear_pin(gpio_t pin) {
    // printf("Set pin to LOW\r\n");
    gpio_clear(pin);
}