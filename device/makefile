APPLICATION ?= hydra_client

BOARD ?= nucleo-f401re

RIOTBASE ?= ../../RIOT

# Change this to 0 remove debug prints:
DEBUG ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# When using I2C, specify the default I2C device to use,
# and the BME280's address (see datasheet).
# The values below are the defaults:
CFLAGS += -DBMX280_PARAM_I2C_DEV=I2C_DEV\(0\)
CFLAGS += -DBMX280_PARAM_I2C_ADDR=0x77

USEMODULE += ztimer
USEMODULE += ztimer_usec

USEMODULE += xtimer

USEMODULE += periph_gpio
USEMODULE += periph_i2c

USEMODULE += analog_util

USEPKG += u8g2

USEMODULE += fmt

# Modules to include:
# USEMODULE += shell
# USEMODULE += shell_commands
# USEMODULE += ps

# BMP280 via I2C
USEMODULE += bmp280_i2c

# set default device parameters in case they are undefined
# TEST_OUTPUT ?= 4

TEST_I2C ?= 0

TEST_ADDR ?= 0x3c

TEST_PIN_CS ?= GPIO_UNDEF
TEST_PIN_DC ?= GPIO_UNDEF
TEST_PIN_RESET ?= GPIO_UNDEF

TEST_DISPLAY ?= u8g2_Setup_sh1106_i2c_128x64_noname_2

FEATURES_REQUIRED += periph_gpio periph_i2c periph_adc

# export parameters
CFLAGS += -DTEST_OUTPUT=$(TEST_OUTPUT)

CFLAGS += -DTEST_I2C=$(TEST_I2C)

CFLAGS += -DTEST_ADDR=$(TEST_ADDR)

CFLAGS += -DTEST_PIN_CS=$(TEST_PIN_CS)
CFLAGS += -DTEST_PIN_DC=$(TEST_PIN_DC)
CFLAGS += -DTEST_PIN_RESET=$(TEST_PIN_RESET)

CFLAGS += -DTEST_DISPLAY=$(TEST_DISPLAY)

# Useful to 'fix' unused anything error
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-unused-variable

include $(RIOTBASE)/Makefile.include
