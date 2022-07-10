#include "core.h"

#include "sh1106.h"
#include "sen0161.h"
#include "bmp280.h"
#include "leds.h"
#include "net.h"

// #include "periph/gpio.h"
// #include "periph/i2c.h"

#include "timex.h"
#include "xtimer.h"
#include "ztimer.h"

// #include "u8g2.h"
// #include "u8x8_riotos.h"

#include "mutex.h"

#ifndef TEST_I2C
#error "TEST_I2C not defined"
#endif
#ifndef TEST_ADDR
#error "TEST_ADDR not defined"
#endif
#ifndef TEST_DISPLAY
#error "TEST_DISPLAY not defined"
#endif
#ifndef TEST_PIN_RESET
#error "TEST_PIN_RESET not defined"
#endif

gpio_t pin_out_r, pin_out_y, pin_out_g;

bmx280_t bmp280;
mutex_t bmp280_lock = MUTEX_INIT;
static char bmp280_thread_stack[THREAD_STACKSIZE_MAIN];

mutex_t sen0161_lock = MUTEX_INIT;
static char sen0161_thread_stack[THREAD_STACKSIZE_DEFAULT];

static char sh1106_thread_stack[THREAD_STACKSIZE_DEFAULT];

static char hydra_thread_stack[THREAD_STACKSIZE_DEFAULT];

static char leds_thread_stack[THREAD_STACKSIZE_DEFAULT];

uint16_t global_temperature;
uint32_t global_pressure;
float global_ph;
state_e global_state;

void on_pub(const emcute_topic_t *topic, void *data, size_t len) {
    char *in = (char *)data;
    printf("### got publication for topic '%s' [%i] ###\n", topic->name, (int)topic->id);

    for(size_t i = 0; i < len; i++) {
        printf("%c", in[i]);
    }

    puts("");
}

void send(char* temp, char* pres, char* ph) {
    printf("VALUES: temp: %f | pres: %f | pH: %f\r\n", atof(temp), atof(pres), atof(ph));

    // const char* msg_struct = "{\"cluster_id\":\"%d\",\"device_id\":\"%s\",\"temperature\":\"%f\",\"pressure\":\"%f\",\"ph\":\"%f\"}";
    // const char* msg_struct  = "{ cluster_id: %d, device_id: %s, temperature: %f, pressure: %f, ph: %f }";
    int length = snprintf(NULL, 0, "{\"cluster_id\":\"%d\",\"device_id\":\"%s\",\"temperature\":\"%s\",\"pressure\":\"%s\",\"ph\":\"%s\"}", 1, EMCUTE_ID, temp, pres, ph);
    char* msg = malloc(length + 1);
    /* TODO: check malloc return value*/

    snprintf(msg, length + 1, "{\"cluster_id\":\"%d\",\"device_id\":\"%s\",\"temperature\":\"%s\",\"pressure\":\"%s\",\"ph\":\"%s\"}", 1, EMCUTE_ID, temp, pres, ph);
    printf("[=] MQTT_TOPIC_OUT: %s\r\n", MQTT_TOPIC_OUT);
    mqtt_pub(MQTT_TOPIC_OUT, msg, 0);

    free(msg);
}

state_e get_status(uint16_t temperature, uint32_t pressure, float pH) {

    int cnt = 0;
    float tmp_temp = temperature / 100;
    float tmp_pres = pressure / 10000;
    printf("[+] %f | %f\r\n", tmp_temp, tmp_pres);
    int temp_error = (tmp_temp >= MIN_TEMP && tmp_temp <= MAX_TEMP) ? 0 : 1;
    printf("[+] temp error: %d\r\n", temp_error);

    int press_error = 0;
    printf("[+] press error: %d\r\n", press_error);

    int ph_error = (pH >= MIN_PH && pH <= MAX_PH) ? 0 : 1;
    printf("[+] ph error: %d\r\n", ph_error);

    cnt = temp_error + press_error + ph_error;
    printf("[+] cnt: %d\r\n", cnt);

    if (cnt == 1) {
        return SOMETHING_WRONG;
    }
    else if (cnt == 2) {
        return PANIC;
    }

    return OK;
}

static void* hydra_thread(void *arg) {
    (void) arg;

    state_e current_state = OK;

    while (1) {

        current_state = get_status(global_temperature, global_pressure, global_ph);
        global_state = current_state;

        send(raw_temperature(global_temperature), raw_pressure(global_pressure), raw_pH(global_ph));
        xtimer_sleep(3);
    }
    
    return 0;
}

static void* leds_thread(void *arg) {
    (void) arg;

    while (1) {
        if (global_state == OK) {

            if (gpio_read(pin_out_g) != 0) {
                continue;
            }
            else if (gpio_read(pin_out_y) != 0) {
                clear_pin(pin_out_g);

                set_pin(pin_out_y);
            }
            else {
                clear_pin(pin_out_g);

                set_pin(pin_out_r);
            }
        }
        else if (global_state == SOMETHING_WRONG) {

            if (gpio_read(pin_out_y) != 0) {
                continue;
            }
            else if (gpio_read(pin_out_g) != 0) {
                clear_pin(pin_out_y);

                set_pin(pin_out_g);
            }
            else {
                clear_pin(pin_out_y);

                set_pin(pin_out_r);
            }
        }
        else {
           if (gpio_read(pin_out_r) != 0) {
                continue;
            }
            else if (gpio_read(pin_out_y) != 0) {
                clear_pin(pin_out_r);

                set_pin(pin_out_y);
            }
            else {
                clear_pin(pin_out_r);

                set_pin(pin_out_g);
            }
        }

        xtimer_sleep(3);
    }
    
    return 0;
}

static void *sh1106_thread(void *u8g2) {
    (void) u8g2;
    uint32_t screen = 0;

    char* tmp_string = NULL;
    char* pres_string = NULL;
    char* ph_string = NULL;

    uint16_t temperature = 0;
    uint32_t pressure = 0;
    float ph = 0.0;

    

    // u8g2_ClearBuffer(u8g2);
    // u8g2_ClearDisplay(u8g2);

    /* use snprintf to convert numbers to strings and calculate their lenght*/
    // https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c

    /* set state given data collected */

    // check pressure
    // check temperature
    // check ph

    /* send them with mqtt? */

    /* display them */

    /* start drawing in a loop */
    printf("[+] Drawing on screen...\r\n");

    while (1) {

        // printf("First page...\r\n");
        u8g2_FirstPage(u8g2);

        do {
            // printf("Next page...\r\n");
            // printf("Draw color...\r\n");
            u8g2_SetDrawColor(u8g2, 1);
            // printf("Set font...\r\n");
            u8g2_SetFont(u8g2, u8g2_font_helvB12_tf);

            // printf("Before switch...\r\n");
            switch (screen) {
                case 0:
                    // printf("Case 0\r\n");
                    u8g2_SetFont(u8g2, u8g2_font_helvB08_tf);
                    u8g2_DrawStr(u8g2, 8, 16, "Welcome to");

                    /* logo print */

                    u8g2_SetFont(u8g2, u8g2_font_helvB12_tf);
                    u8g2_DrawStr(u8g2, 54, 40, "HYDRA");
                    break;
                case 1:
                    // printf("Case 3\r\n"); 
                    u8g2_SetFont(u8g2, u8g2_font_helvB08_tf);
                    u8g2_DrawStr(u8g2, 8, 16, "Powered by");
                    u8g2_DrawBitmap(u8g2, 54, 20, 8, 32, riot_logo);
                    break;
                case 2:

                    // temperature = get_env_raw_temperature(&bmp280);
                    tmp_string = str_temperature(global_temperature);
                    printf("[+] Temperature string: %s\r\n", tmp_string);

                    // pressure = get_env_raw_pressure(&bmp280);
                    pres_string = str_pressure(global_pressure);
                    printf("[+] Pressure string: %s\r\n", pres_string);

                    // printf("Case 2\r\n");
                    u8g2_DrawStr(u8g2, 6, 12, tmp_string);
                    u8g2_DrawStr(u8g2, 6, 36, pres_string);

                    free(tmp_string);
                    free(pres_string);
                    break;
                case 3:
                    // printf("Case 2\r\n");
                    // ph = get_ph_value();
                    ph_string = str_pH(global_ph);
                    printf("[+] pH string: %s\r\n", ph_string);

                    u8g2_DrawStr(u8g2, 6, 12, ph_string);

                    free(ph_string);
                    break;
                case 4:
                    // printf("Case 2\r\n");
                    u8g2_SetFont(u8g2, u8g2_font_helvB08_tf);

                    if (global_state == OK) {
                        u8g2_DrawStr(u8g2, 6, 12, "Everything is all right!");
                    }
                    else if (global_state == SOMETHING_WRONG) {
                        u8g2_DrawStr(u8g2, 6, 12, "We have an error...");
                    }
                    else {
                        u8g2_DrawStr(u8g2, 6, 12, "Call the boss!!!");
                    }
                    
                    // u8g2.setBufferCurrTileRow(0);       // let y=0 be the topmost row of the buffer
                    // u8g2.clearBuffer();
                    // u8g2.setFont(u8g2_font_helvB08_tr);
                    // u8g2.drawStr(2, 8, "abcdefg");

                    // u8g2.setBufferCurrTileRow(2);	// write the buffer to tile row 2 (y=16) on the display
                    // u8g2.sendBuffer();
                    // u8g2.setBufferCurrTileRow(4);	// write the same buffer to tile row 4 (y=32) on the display
                    // u8g2.sendBuffer();
                    break;
            }
            // printf("After switch...\r\n");

        } while (u8g2_NextPage(u8g2));

        /* show screen in next iteration */
        screen = (screen + 1) % 5;

        /* sleep a little */
        // ztimer_sleep(ZTIMER_USEC, US_PER_SEC);
        xtimer_sleep(2);
        // usleep(1);
    }

    // free(str_temp);
    // free(tmp_string);
    // free(ph_string);

    return 0;
}

static void *sen0161_thread(void *arg) {
    (void) arg;

    while (1) {

        /* acquire mtx */
        mutex_lock(&sen0161_lock);

        int sample = 0;
        float mapped_voltage = 0.0;
        float ph = 0.0;
        float min_ph_value = 0; // pH is Adimensional, Turbidity is 0 mg/L
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
        printf("Voltage read: %f V\r\n", mapped_voltage);
#endif

        ph = adc_util_mapf(sample, ADC_RES, min_ph_value, max_ph_value);

#ifndef DEBUG
        printf("pH values: %2f\r\n", ph);
#endif

        if (sample < 0) {
            printf("ADC_LINE(%u): selected resolution not applicable\r\n", ADC_IN_USE);
        }

#ifdef DEBUG
        printf("ADC_LINE(%u): raw value: %i, ph: %f\r\n", ADC_IN_USE, sample, ph);
#endif

        global_ph = ph;
        
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
            float pres0 = 1013.25;
            float base = (pres / pres0);
            float exp = (1 / 5.255);
            float p = pow(base, exp);
            float h = 44330 * (1 - p);
            // printf("[=] Altitude from pressure: %lfm \r\n", h);

            // update global value
            global_temperature = temp;
            global_pressure = pres;
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

static int bmp280_handler(int argc, char *argv[]) {
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
        // float pres0 = 1013.25;
        // float base = (pres / pres0);
        // float exp = (1 / 5.255);
        // float p = pow(base, exp);
        // float h = 44330 * (1 - p);
        // printf("[=] Altitude from pressure: %fm\r\n", h);
    }
    else {
        _bmp280_usage(argv[0]);
        return -1;
    }
    
    return 0;
}

static int sen0161_handler(int argc, char *argv[]) {
    int sample = 0;
    float mapped_voltage = 0;
    float ph = 0.0;
    float min_ph_value = 0; // pH is adimensional
    float max_ph_value = 14.0;

    sample = adc_sample(ADC_IN_USE, ADC_RES);
    if (sample == -1) { // Error with sample resolution
        printf("[-] ADC_LINE(0): selected resolution not applicable\n");
        return (sample); //In this case sample contains Error code
    }

#ifdef DEBUG
    printf("[+] Sampled bit (raw sample) from pin A0: %d\n", sample);
#endif
    mapped_voltage = sample * (5.0 / pow(2, 12));
#ifdef DEBUG
    printf("[+] Voltage read: %.2f V\r\n", mapped_voltage);
#endif
    ph = adc_util_mapf(sample, ADC_RES, min_ph_value, max_ph_value);
#ifdef DEBUG
    printf("[+] pH values: %.2f \r\n", ph);
#endif

    return 0;
}

static const shell_command_t commands[] = {
    {"bmp","Read BMP280 values", bmp280_handler},
    {"ph", "Read pH value", sen0161_handler},
    // {"bmc","Periodically reads BMP280 values", bmp280_thread_handler},
    { NULL, NULL, NULL }
};

int main(void) {
    printf("[+] This is %s on %s!\r\n", RIOT_APPLICATION, RIOT_BOARD);

    u8g2_t u8g2;
    tank_t *hydra_tank;
    // gpio_t pin_out_r = 0, pin_out_y = 0, pin_out_g = 0;

    // init_leds(pin_out_r, pin_out_y, pin_out_g);
    // init_bmp280(bmp280);
    // init_oled_display(u8g2);
    init_sen0161();

    printf("[+] Init pins' leds\r\n");
    pin_out_r = GPIO_PIN(PORT_B, 4);
    // red = GPIO_PIN(PORT_B, 4);
    if (gpio_init(pin_out_r, GPIO_OUT)) {
        printf("[-] Error to initialize GPIO_PIN(%d %d) (aka red led)\r\n", PORT_B, 4);
        return -1;
    }

    pin_out_y = GPIO_PIN(PORT_B, 5);
    // yellow = GPIO_PIN(PORT_B, 5);
    if (gpio_init(pin_out_y, GPIO_OUT)) {
        printf("[-] Error to initialize GPIO_PIN(%d %d) (aka yellow led)\r\n", PORT_B, 5);
        return -1;
    }

    pin_out_g = GPIO_PIN(PORT_B, 3);
    // green = GPIO_PIN(PORT_B, 3);
    if (gpio_init(pin_out_g, GPIO_OUT)) {
        printf("[-] Error to initialize GPIO_PIN(%d %d) (aka green led)\r\n", PORT_B, 3);
        return -1;
    }

    /* initialize bmp280 */
    printf("[+] Init bmp280\r\n");
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
    u8g2_ClearDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);

    /* Prepare tank components and init the struct */
    int tank_id = 1;
    // time_t rawtime;
    // time(&rawtime);
    // int timeinfo = localtime(&rawtime);

    // environment_t env = {
    //     .id = tank_id,
    //     .bmp280 = bmp280,
    //     .last_update = 0
    // };

    // nutrient_solution_t sol = {
    //     .id = tank_id,
    //     .ph = 7,
    //     .last_update = 0
    // };

    tank_t tnk = {
        .id = tank_id,
        .env = { tank_id, &bmp280, 0 },
        .solution = { tank_id, NEUTRAL_PH, 0 },
        // .display = u8g2,
        .state = DEFAULT_STATE,
        .last_update = 0
    };
    memcpy(&tnk.display, &u8g2, sizeof(u8g2_t));

    // memcpy(&hydra_tank, &tnk, sizeof(tank_t));
    
    // memcpy(&tnk.env.sensor, &bmp280, sizeof(bmx280_t));

    printf("[+] Doing some tests with semaphore...\r\n");
    set_pin(pin_out_r);
    // xtimer_sleep(1);
    // clear_pin(pin_out_r);

    // set_pin(pin_out_y);
    // xtimer_sleep(1);
    // clear_pin(pin_out_y);

    // set_pin(pin_out_g);
    // xtimer_sleep(1);
    // clear_pin(pin_out_g);

    setup_mqtt(on_pub);

    /* TODO: handle thread failure? */
    thread_create(bmp280_thread_stack, sizeof(bmp280_thread_stack), THREAD_PRIORITY_MAIN - 1, 0, bmp280_thread, NULL, "bmp280");
    thread_create(sen0161_thread_stack, sizeof(sen0161_thread_stack), THREAD_PRIORITY_MAIN - 1, 0, sen0161_thread, NULL, "sen0161");
    thread_create(leds_thread_stack, sizeof(leds_thread_stack), THREAD_PRIORITY_MAIN - 1, 0, leds_thread, NULL, "leds");

    // thread_create(sh1106_thread_stack, sizeof(sh1106_thread_stack), THREAD_PRIORITY_MAIN - 1, 0, sh1106_thread, (void*) &u8g2, "sh1106");
    // thread_create(hydra_thread_stack, sizeof(hydra_thread_stack), THREAD_PRIORITY_MAIN - 1, 0, hydra_thread, hydra_tank, "hydra_dev");
    xtimer_sleep(3);
    thread_create(sh1106_thread_stack, sizeof(sh1106_thread_stack), THREAD_PRIORITY_MAIN - 1, 0, sh1106_thread, (void*) &tnk.display, "sh1106");

    xtimer_sleep(3);
    thread_create(hydra_thread_stack, sizeof(hydra_thread_stack), THREAD_PRIORITY_MAIN - 1, 0, hydra_thread, NULL, "hydra_dev");

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
