#include "sen0161.h"

int init_sen0161(void) {
    /* init the ADC line */
    if (adc_init(ADC_IN_USE) < 0) {
        printf("[-] Initialization of ADC_LINE(%u) failed\n", ADC_IN_USE);
        return 1;
    }

#ifdef DEBUG
    printf("[+] Successfully initialized ADC_LINE(%u)...\r\n", ADC_IN_USE);
#endif

    return 0;
}

float get_ph_value(void) {
    // int32_t sample = 0;
    // double mapped_voltage = 0.0;
    // double ph = 0.0;
    // double min_ph_value = 0.0;
    // double max_ph_value = 14.0;

    // TODO: try to switch float to double in order to calculate a float value for ph
    int sample = 0;
    float mapped_voltage = 0.0;
    float ph = 0.0;
    float min_ph_value = 0; // pH is Adimensional, Turbidity is 0 mg/L
    float max_ph_value = 14.0;

    sample = adc_sample(ADC_IN_USE, ADC_RES);
    if (sample == -1) { // Error with sample resolution
        printf("[-] ADC_LINE(0): selected resolution not applicable\n");
        return (sample); //In this case sample contains Error code
    }

#ifdef DEBUG
    printf("Sampled bit (raw sample) from pin A00: %d\n", sample);
#endif

    mapped_voltage = sample * (5.0 / pow(2, 12));

#ifdef DEBUG
    printf("Voltage read: %f V\r\n", mapped_voltage);
#endif

    ph = adc_util_mapf(sample, ADC_RES, min_ph_value, max_ph_value);

#ifndef DEBUG
    printf("pH values: %2f\r\n", ph);
#endif

    if (sample < 0) {
        printf("ADC_LINE(%u): selected resolution not applicable\r\n", ADC_IN_USE); // FIXME: maybe handle this case gracefully
    }

#ifdef DEBUG
    printf("ADC_LINE(%u): raw value: %i, ph: %f\r\n", ADC_IN_USE, sample, ph);
#endif

    return ph;
}

char* raw_pH(float ph) {
    int length = snprintf(NULL, 0, "%.2f", ph);
    char* s = malloc(length + 1);
    /* TODO: check malloc return value*/

    snprintf(s, length + 1, "%.2f", ph);

    return s;
}

char* str_pH(float ph) {
    int length = snprintf(NULL, 0, "pH: %.2f", ph);
    char* s = malloc(length + 1);
    /* TODO: check malloc return value*/

    snprintf(s, length + 1, "pH: %.2f", ph);

    return s;
}