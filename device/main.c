#include "core.h"

#include "sh1106.h"
#include "sen0161.h"
#include "bmp280.h"

#include "periph/gpio.h"
#include "periph/i2c.h"
#include "periph/adc.h"

#include "analog_util.h"

#include "net/emcute.h"

#include "timex.h"
#include "xtimer.h"
#include "ztimer.h"

#include "u8g2.h"
#include "u8x8_riotos.h"

#include "bmx280.h"
#include "bmx280_params.h"

#include "mutex.h"

#include "net.h"

// #ifndef TEST_OUTPUT
// #error "TEST_OUTPUT not defined"
// #endif

// #ifndef TEST_I2C
// #error "TEST_I2C not defined"
// #endif
// #ifndef TEST_ADDR
// #error "TEST_ADDR not defined"
// #endif
// #ifndef TEST_DISPLAY
// #error "TEST_DISPLAY not defined"
// #endif
// #ifndef TEST_PIN_RESET
// #error "TEST_PIN_RESET not defined"
// #endif

static bmx280_t bmp280;
static mutex_t bmp280_lock = MUTEX_INIT;
static char bmp280_thread_stack[THREAD_STACKSIZE_MAIN];

static mutex_t sen0161_lock = MUTEX_INIT;
static char sen0161_thread_stack[THREAD_STACKSIZE_DEFAULT];

static char sh1106_thread_stack[THREAD_STACKSIZE_DEFAULT];

void set_pin(gpio_t pin) {
    printf("Set pin to HIGH\r\n");
    gpio_set(pin);
}

void clear_pin(gpio_t pin) {
    printf("Set pin to LOW\r\n");
    gpio_clear(pin);
}

void on_pub(const emcute_topic_t *topic, void *data, size_t len) {
    char *in = (char *)data;
    printf("### got publication for topic '%s' [%i] ###\n",
           topic->name, (int)topic->id);

    for(size_t i = 0; i < len; i++) {
        printf("%c", in[i]);
    }
    puts("");

}

void send(int val) {
    printf("VALUE: %d\n",val);
    char str[50];
    sprintf(str,"{\"id\":\"%s\",\"pressed_key\":\"%d\"}",EMCUTE_ID,val);
    mqtt_pub(MQTT_TOPIC_OUT,str,0);
}

// static void *sh1106_thread(void *arg) {
//     (void) arg;

//     uint32_t screen = 0;
//     u8g2_t u8g2 = (u8g2_t) arg;

//     while (1) {
//         printf("First page...\r\n");
//         u8g2_FirstPage(&u8g2);

//         do {
//             printf("Next page...\r\n");
//             printf("Draw color...\r\n");
//             u8g2_SetDrawColor(&u8g2, 1);
//             printf("Set font...\r\n");
//             u8g2_SetFont(&u8g2, u8g2_font_helvB12_tf);

//             printf("Before switch...\r\n");
//             switch (screen) {
//                 case 0:
//                     printf("Case 0\r\n");
//                     u8g2_DrawStr(&u8g2, 12, 22, "THIS");
//                     break;
//                 case 1:
//                     printf("Case 1\r\n");
//                     u8g2_DrawStr(&u8g2, 24, 22, "IS");
//                     break;
//                 case 2:
//                     printf("Case 2\r\n");
//                     u8g2_DrawStr(&u8g2, 12, 22, "HYDRA");
//                     break;
//                 case 3:
//                     printf("Case 3\r\n");
//                     u8g2_DrawBitmap(&u8g2, 0, 0, 8, 32, riot_logo);
//                     break;
//             }

//             printf("After switch...\r\n");

//         } while (u8g2_NextPage(&u8g2));

//         printf("Before show...\r\n");

//         /* show screen in next iteration */
//         screen = (screen + 1) % 4;

//         /* sleep a little */
//         // ztimer_sleep(ZTIMER_USEC, US_PER_SEC);
//         xtimer_sleep(3);
//     }

//     return 0;
// }

static void *sen0161_thread(void *arg) {
    (void) arg;

    while (1) {

        /* acquire mtx */
        mutex_lock(&sen0161_lock);

        /* ... */
        int sample = 0;
        int mapped_voltage = 0;
        int ph = 0;
        float min_ph_value = 0; // pH is adimensional
        float max_ph_value = 14.0;

        sample = adc_sample(ADC_IN_USE, ADC_RES);
        if (sample == -1) { // Error with sample resolution
            printf("[-] ADC_LINE(0): selected resolution not applicable\n");
            // return (sample); //In this case sample contains Error code
        }

#ifdef DEBUG
        printf("Sampled bit (raw sample) from pin A0: %d\n", sample);
#endif
        mapped_voltage = sample * (5.0 / pow(2, 12));
#ifdef DEBUG
        printf("Voltage read: %d V\r\n", mapped_voltage);
#endif
        ph = adc_util_mapf(sample, ADC_RES, min_ph_value, max_ph_value);
#ifdef DEBUG
        printf("pH values: %i \r\n", ph);
#endif
        printf("[=] pH values: %i \r\n", ph);

        if (sample < 0) {
            printf("ADC_LINE(%u): selected resolution not applicable\r\n", ADC_IN_USE);
        }

#ifdef DEBUG
    printf("ADC_LINE(%u): raw value: %i, ph: %i\r\n", ADC_IN_USE, sample, ph);
#endif
        
        /* release mtx*/
        mutex_unlock(&sen0161_lock);
        xtimer_sleep(3);
    }

    return 0;
}

static void *bmp280_thread(void *arg) {
    (void) arg;

    while (1) {

        /* acquire mtx */
        mutex_lock(&bmp280_lock);

        /* read temperature */
        int16_t temp = 0;
        temp = bmx280_read_temperature(&bmp280);
        // todo: check return value
        if (temp == INT16_MIN) {
            printf("[-] Temperature value read did not succeed!\r\n");
            mutex_unlock(&bmp280_lock);
            exit(127);
        }
        else {

            printf("[=] Temperature: %i.%u C\r\n", (temp/100), (temp%100));

            uint32_t pres = 0;
            pres = bmx280_read_pressure(&bmp280);

            printf("[=] Pressure: %li.%lu kPa\r\n", (pres/1000), (pres%1000));
            printf("[=] Pressure: %li.%lu bar\r\n", (pres/100000), (pres%100000));
            
            // H = 44330 * [ 1 - (P / p0) ^ (1 / 5,255) ]
            double pres0 = 1013.25;
            double base = (pres / pres0);
            double exp = (1 / 5.255);
            double p = pow(base, exp);
            double h = 44330 * (1 - p);
            // printf("[=] Altitude from pressure: %lfm \r\n", h);
        }

        /* release mtx*/
        mutex_unlock(&bmp280_lock);
        xtimer_sleep(3);
    }

    return 0;
}

static void _bmp280_usage(char *cmd){
    printf("[?] Usage: %s [temperature | pressure | all]\n", cmd);
}

int bmp280_handler(int argc, char *argv[]) {
    (void) argc;

    if (!strcmp(argv[1],"temperature")) {
        int16_t temp = 0;
        temp = bmx280_read_temperature(&bmp280);
        printf("[=] Temperature: %i.%u C\r\n",(temp/100),(temp%100));
    }
    else if (!strncmp(argv[1],"pressure", strlen("pressure"))) { 
        bmx280_read_temperature(&bmp280);

        uint32_t pres = 0;
        pres = bmx280_read_pressure(&bmp280);
        printf("[=] Pressure: %li.%lu kPa\r\n", (pres/1000), (pres%1000));
        printf("[=] Pressure: %li.%lu bar\r\n", (pres/100000), (pres%100000));
    }
    else if (!strncmp(argv[1],"all", strlen("all"))) {
        /*
            This function returns the pressure data that was measured when bmx280_read_temperature() has been called last. 
            So bmx280_read_temperature() has to be called before. 
            If bmx280_read_temperatue() did not succeed in acquiring a new set of sensor data, 
            the result of this function is undefined.
        */
        
        int16_t temp = 0;
        temp = bmx280_read_temperature(&bmp280);
        printf("[=] Temperature: %i.%u C\r\n", (temp/100), (temp%100));

        uint32_t pres = 0;
        pres = bmx280_read_pressure(&bmp280);

        printf("[=] Pressure: %li.%lu kPa\r\n", (pres/1000), (pres%1000));
        printf("[=] Pressure: %li.%lu bar\r\n", (pres/100000), (pres%100000));
        
        // H = 44330 * [ 1 - (P / p0) ^ (1 / 5,255) ]
        double pres0 = 1013.25;
        double base = (pres / pres0);
        double exp = (1 / 5.255);
        double p = pow(base, exp);
        double h = 44330 * (1 - p);
        printf("[=] Altitude from pressure: %lfm \r\n", h);
    }
    else {
        _bmp280_usage(argv[0]);
        return -1;
    }
    
    return 0;
}

static int ph_value(void) {
    // xtimer_ticks32_t last = xtimer_now();

    // int32_t sample = 0;
    // double mapped_voltage = 0.0;
    // double ph = 0.0;
    // double min_ph_value = 0.0;
    // double max_ph_value = 14.0;

    // TODO: try to switch float to double in order to calculate a float value for ph
    int sample = 0;
    int mapped_voltage = 0;
    int ph = 0;
    float min_ph_value = 0; // pH is Adimensional, Turbidity is 0 mg/L
    float max_ph_value = 14.0;

    sample = adc_sample(ADC_IN_USE, ADC_RES);
    if (sample == -1) { // Error with sample resolution
        printf("[-] ADC_LINE(0): selected resolution not applicable\n");
        return (sample); //In this case sample contains Error code
    }

#ifdef DEBUG
    printf("Sampled bit (raw sample) from pin A0: %d\n", sample);
#endif
    mapped_voltage = sample * (5.0 / pow(2, 12));
#ifdef DEBUG
    printf("Voltage read: %d V\r\n", mapped_voltage);
#endif
    ph = adc_util_mapf(sample, ADC_RES, min_ph_value, max_ph_value);
#ifdef DEBUG
    printf("pH values: %i \r\n", ph);
#endif

    if (sample < 0) {
        printf("ADC_LINE(%u): selected resolution not applicable\r\n", ADC_IN_USE);
    }

#ifdef DEBUG
    printf("ADC_LINE(%u): raw value: %i, ph: %i\r\n", ADC_IN_USE, sample, ph);
#endif

    // while (1) {

    //     xtimer_periodic_wakeup(&last, DELAY);
    // }

    return 0;
}

static int bmp280_values(void) {
    int16_t temp = 0;
    temp = bmx280_read_temperature(&bmp280);
#ifdef DEBUG
    printf("[=] Temperature: %i.%u C\r\n", (temp/100), (temp%100));
#endif
    uint32_t pres = 0;
    pres = bmx280_read_pressure(&bmp280);

#ifdef DEBUG
    // printf("[=] Pressure: %li.%lu kPa\r\n", (pres/1000), (pres%1000));
    printf("[=] Pressure: %li.%lu bar\r\n", (pres/100000), (pres%100000));
#endif
        
    // H = 44330 * [ 1 - (P / p0) ^ (1 / 5,255) ]
    double pres0 = 1013.25;
    double base = (pres / pres0);
    double exp = (1 / 5.255);
    double p = pow(base, exp);
    double h = 44330 * (1 - p);
#ifdef DEBUG
    printf("[=] Altitude from pressure: %lfm \r\n", h);
#endif

    return 0;
}

static const shell_command_t commands[] = {
    {"bmp","Read BMP280 values", bmp280_handler},
    // {"bmc","Periodically reads BMP280 values", bmp280_thread_handler},
    { NULL, NULL, NULL }
};

int main(void) {
    uint32_t screen = 0;
    u8g2_t u8g2;

    printf("[+] This is %s on %s!\r\n", RIOT_APPLICATION, RIOT_BOARD);

    /* init semaphore leds */
    printf("[+] Initializing semaphore leds...\r\n");
    gpio_t pin_out_r = GPIO_PIN(PORT_B, 4);
    if (gpio_init(pin_out_r, GPIO_OUT)) {
        printf("[-] Error to initialize GPIO_PIN(%d %d) (aka red led)\r\n", PORT_B, 4);
        return -1;
    }

    gpio_t pin_out_y = GPIO_PIN(PORT_B, 5);
    if (gpio_init(pin_out_y, GPIO_OUT)) {
        printf("[-] Error to initialize GPIO_PIN(%d %d) (aka yellow led)\r\n", PORT_B, 5);
        return -1;
    }

    gpio_t pin_out_g = GPIO_PIN(PORT_B, 3);
    if (gpio_init(pin_out_g, GPIO_OUT)) {
        printf("[-] Error to initialize GPIO_PIN(%d %d) (aka green led)\r\n", PORT_B, 3);
        return -1;
    }

    printf("[+] Doing some tests with semaphore...\r\n");
    set_pin(pin_out_r);
    xtimer_sleep(1);
    clear_pin(pin_out_r);

    set_pin(pin_out_y);
    xtimer_sleep(1);
    clear_pin(pin_out_y);

    set_pin(pin_out_g);
    xtimer_sleep(1);
    clear_pin(pin_out_g);

    // xtimer_sleep(2);

    // clear_pin(pin_out_r);
    // clear_pin(pin_out_y);
    // clear_pin(pin_out_g);

    xtimer_sleep(2);

    /* init the ADC line */
    if (adc_init(ADC_IN_USE) < 0) {
        printf("[-] Initialization of ADC_LINE(%u) failed\n", ADC_IN_USE);
        return 1;
    }

#ifndef DEBUG
    printf("[+] Successfully initialized ADC_LINE(%u)...\r\n", ADC_IN_USE);
#endif

    ph_value();

    xtimer_sleep(5);

    /* init bmp280 sensor */
    printf("[+] Initializing bmp280 sensor...\r\n");
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
    else { printf("[X] WTF is going on?\r\n"); }

    setup_mqtt(on_pub);

    printf("[+] Reading bmp280 sensor values...\r\n");
    bmp280_values();

    xtimer_sleep(3);

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

    /* start drawing in a loop */
    // printf("Drawing on screen.\r\n");

    thread_create(bmp280_thread_stack, sizeof(bmp280_thread_stack), THREAD_PRIORITY_MAIN - 1, 0, bmp280_thread, NULL, "bmp280");
    thread_create(sen0161_thread_stack, sizeof(sen0161_thread_stack), THREAD_PRIORITY_MAIN - 1, 0, sen0161_thread, NULL, "sen0161");
    // thread_create(sh1106_thread_stack, sizeof(sh1106_thread_stack), THREAD_PRIORITY_MAIN - 1, 0, sh1106_thread, (void*) u8g2, "sh1106");

    while (1) {
        printf("First page...\r\n");
        u8g2_FirstPage(&u8g2);

        do {
            printf("Next page...\r\n");
            printf("Draw color...\r\n");
            u8g2_SetDrawColor(&u8g2, 1);
            printf("Set font...\r\n");
            u8g2_SetFont(&u8g2, u8g2_font_helvB12_tf);

            printf("Before switch...\r\n");
            switch (screen) {
                case 0:
                    printf("Case 0\r\n");
                    u8g2_DrawStr(&u8g2, 12, 22, "THIS");
                    break;
                case 1:
                    printf("Case 1\r\n");
                    u8g2_DrawStr(&u8g2, 24, 22, "IS");
                    break;
                case 2:
                    printf("Case 2\r\n");
                    u8g2_DrawStr(&u8g2, 12, 22, "HYDRA");
                    break;
                case 3:
                    printf("Case 3\r\n");
                    u8g2_DrawBitmap(&u8g2, 0, 0, 8, 32, riot_logo);
                    break;
            }
            printf("After switch...\r\n");
        } while (u8g2_NextPage(&u8g2));

        printf("Before show...\r\n");

        /* show screen in next iteration */
        screen = (screen + 1) % 4;

        /* sleep a little */
        // ztimer_sleep(ZTIMER_USEC, US_PER_SEC);
        xtimer_sleep(3);
    }

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
