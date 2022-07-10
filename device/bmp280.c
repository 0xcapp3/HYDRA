#include "bmp280.h"

int init_bmp280(bmx280_t bmp280) {
    int res = bmx280_init(&bmp280, bmx280_params);
    
    if (res == BMX280_OK) {
        printf("[+] BMP280 initialized succesfully...\r\n");
    }
    else if (res == BMX280_ERR_BUS) {
        printf("[-] BMP280 bus error!\r\n");
        return (BMX280_ERR_BUS);
    }
    else if (res == BMX280_ERR_NODEV) {
        printf("[-] BMP280 device not found!\r\n");
        return (BMX280_ERR_NODEV);
    }
    else { 
        printf("[X] WTF is going on?\r\n"); 
        return 127;
    }

    return (BMX280_OK);
}

uint16_t get_env_raw_temperature(bmx280_t* bmp280) {
    int16_t temp = 0;
    temp = bmx280_read_temperature(bmp280);

    return temp;
}

char* raw_temperature(uint16_t temp) {
    int len = snprintf(NULL, 0, "%i.%u", (temp/100), (temp%100));
    char* raw = malloc(len + 1);
    /* TODO: check malloc return value*/

    snprintf(raw, len + 1, "%i.%u", (temp/100), (temp%100));
    return raw;
}

char* str_temperature(uint16_t temp) {
    int len = snprintf(NULL, 0, "Temp: %i.%u C", (temp/100), (temp%100));
    char* temperature = malloc(len + 1);
    /* TODO: check malloc return value*/

    snprintf(temperature, len + 1, "Temp: %i.%u C", (temp/100), (temp%100));
    return temperature;
}

uint32_t get_env_raw_pressure(bmx280_t* bmp280) {
    uint32_t pres = 0;
    pres = bmx280_read_pressure(bmp280);

    return pres;
}

char* raw_pressure(uint32_t pres) {
    int len = snprintf(NULL, 0, "%li.%.lu", (pres/100000), (pres%100));
    char* raw = malloc(len + 1);
    /* TODO: check malloc return value*/

    snprintf(raw, len + 1, "%li.%.lu", (pres/100000), (pres%100));
    return raw;
}

char* str_pressure(uint32_t pres) {
    int len = snprintf(NULL, 0, "Pres: %li.%.lu bar", (pres/100000), (pres%100));
    char* pressure = malloc(len + 1);
    /* TODO: check malloc return value*/

    snprintf(pressure, len + 1, "Pres: %li.%.lu bar", (pres/100000), (pres%100));
    return pressure;
}