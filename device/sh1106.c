#include "sh1106.h"

int init_oled_display(u8g2_t u8g2) {
    /* initialize to I2C */
    printf("[+] Initializing oled display to I2C...\r\n");

    TEST_DISPLAY(&u8g2, U8G2_R0, u8x8_byte_hw_i2c_riotos, u8x8_gpio_and_delay_riotos);

    u8x8_riotos_t hydra_data = {
        .device_index = I2C_DEV(0),
        .pin_cs = TEST_PIN_CS,
        .pin_dc = TEST_PIN_DC,
        .pin_reset = TEST_PIN_RESET,
    };

    u8g2_SetUserPtr(&u8g2, &hydra_data);
    u8g2_SetI2CAddress(&u8g2, TEST_ADDR);

    /* initialize the display */
    printf("Initializing oled display...\r\n");

    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);

    u8g2_ClearBuffer(&u8g2);

    return 0;
}