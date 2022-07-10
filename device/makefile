APPLICATION ?= hydra_device

BOARD ?= nucleo-f401re

RIOTBASE ?= ../../RIOT

# Change this to 0 remove debug prints:
DEBUG ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6
USEMODULE += gnrc_ipv6_default
# Include MQTT-SN
USEMODULE += emcute
# Optimize network stack to for use with a single network interface
USEMODULE += gnrc_netif_single
USEMODULE += stdio_ethos gnrc_uhcpc

# TODO: check if it print pH float value
USEMODULE += printf_float

EMCUTE_ID ?= hydra_dev0
ETHOS_BAUDRATE ?= 115200
TAP ?= tap0
USE_DHCPV6 ?= 0
IPV6_PREFIX ?= fe80:2::/64
SERVER_ADDR = fec0:affe::1
SERVER_PORT = 1885
MQTT_TOPIC_OUT = hydra_uplink
MQTT_TOPIC_IN = hydra_downlink

CFLAGS += -DSERVER_ADDR='"$(SERVER_ADDR)"'
CFLAGS += -DSERVER_PORT='$(SERVER_PORT)'
CFLAGS += -DMQTT_TOPIC_OUT='"$(MQTT_TOPIC_OUT)"'
CFLAGS += -DMQTT_TOPIC_IN='"$(MQTT_TOPIC_IN)"'

# Allow for env-var-based override of the nodes name (EMCUTE_ID)
ifneq (,$(EMCUTE_ID))
  CFLAGS += -DEMCUTE_ID=\"$(EMCUTE_ID)\"
endif

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
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

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
CFLAGS += -Wno-unused-but-set-variable

include $(CURDIR)/makefile.ethos.conf
include $(RIOTBASE)/Makefile.include


.PHONY: host-tools

host-tools:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)
