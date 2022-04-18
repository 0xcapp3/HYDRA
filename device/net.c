#include <stdint.h>
#include <stdio.h>

#include "net/ipv6/addr.h"
#include "net/emcute.h"

#include "net.h"

#define _IPV6_DEFAULT_PREFIX_LEN        (64U)
#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 2)
#define NUMOFSUBS           (16U)
#define TOPIC_MAXLEN        (64U)

static emcute_sub_t subscriptions[NUMOFSUBS];
static char emcute_stack[THREAD_STACKSIZE_MAIN];

static void *emcute_thread(void *arg) {
    (void)arg;
    emcute_run(CONFIG_EMCUTE_DEFAULT_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}

/* import "ifconfig" shell command, used for printing addresses */
extern int _gnrc_netif_config(int argc, char **argv);

static int netif_add(char* addr) {
    char* args[] = {"ifconfig", "4", "add", addr};
    _gnrc_netif_config(4, args);

    return 0;
}

int setup_mqtt(on_pub_handler_t on_pub) {

    memset(subscriptions, 0, sizeof(subscriptions));

    // start the emcute thread
    thread_create(emcute_stack, sizeof(emcute_stack), EMCUTE_PRIO, 0, emcute_thread, NULL, "emcute");

    // Adding address to network interface
    netif_add("fec0:affe::99");

    // connect to MQTT-SN broker
    printf("Connecting to MQTT-SN broker %s port %d.\n", SERVER_ADDR, SERVER_PORT);

    sock_udp_ep_t gw = {
        .family = AF_INET6,
        .port = SERVER_PORT
    };

    // parse address
    if(ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, SERVER_ADDR) == NULL) {
        printf("error parsing IPv6 address\n");
        return 1;
    }

    if(emcute_con(&gw, true, NULL, NULL, 0, 0) != EMCUTE_OK) {
        printf("error: unable to connect to [%s]:%i\n", SERVER_ADDR, (int)gw.port);
        return 1;
    }

    printf("Successfully connected to gateway at [%s]:%i\n", SERVER_ADDR, (int)gw.port);

    // setup subscription to topic
    unsigned flags = EMCUTE_QOS_0;
    subscriptions[0].cb = on_pub;
    subscriptions[0].topic.name = MQTT_TOPIC_IN;

    if(emcute_sub(&subscriptions[0], flags) != EMCUTE_OK) {
        printf("error: unable to subscribe to %s\n", MQTT_TOPIC_IN);
        return 1;
    }

    printf("Now subscribed to %s\n", MQTT_TOPIC_IN);
    return 0;
}

int mqtt_pub(char* topic, const char* data, int qos) {
    emcute_topic_t t;

    unsigned flags = EMCUTE_QOS_0;
    if(qos == 1) flags = EMCUTE_QOS_1;
    else if(qos == 2) flags = EMCUTE_QOS_2;

    t.name = topic;
    if(emcute_reg(&t) != EMCUTE_OK) {
        printf("PUB ERROR: Unable to obtain Topic ID, %s",topic);
        return 1;
    }
    if(emcute_pub(&t, data, strlen(data), flags) != EMCUTE_OK) {
        printf("PUB ERROR: unable to publish data to topic '%s [%i]'\n", t.name, (int)t.id);
        return 1;
    }

    printf("PUB SUCCESS: Published %s on topic %s\n", data, topic);
    return 0;
}

