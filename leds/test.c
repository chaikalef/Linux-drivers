#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define HELLO_MAJOR 250

#define LEDCMD_RESET_STATE _IO(HELLO_MAJOR, 1)
#define LEDCMD_GET_STATE _IOR(HELLO_MAJOR, 2, unsigned char *)
#define LEDCMD_GET_LED_STATE _IOWR(HELLO_MAJOR, 3,  led_t *)
#define LEDCMD_SET_LED_STATE _IOW(HELLO_MAJOR, 4, led_t *)

/* define name of the drivers */
#define FILENAME "/dev/leds"

/* initial state of LEDs */
#define INITIAL_STATE 0x00

#define BP "%d%d%d%d%d%d%d%d"
#define BB(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0)


typedef struct led {
    int pin;
    unsigned char value;
} led_t;

#define STATE_ALL -1

void print_usage(void);
void led_reset(void);
void led_state(int led_n);
void led_off(int led_n);
void led_on(int led_n);


void led_reset(void) {
    int fd;
    /* open device /dev/leds */
    fd = open(FILENAME, O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "Cannot open file %s\n", FILENAME);
        return;
    }

    if (ioctl(fd, LEDCMD_RESET_STATE) == -1) {
        fprintf(stderr, "RESET ERROR!\n");
        return;
    }
    printf("Сброс индикаторов\n");
    close(fd);
}

void led_state(int led_n) {
    int fd;
    led_t led;
    /* state of all LEDs at once in one byte */
    char state;

    /* open device /dev/leds */
    fd = open(FILENAME, O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "Cannot open file %s\n", FILENAME);
        return;
    }

    if (led_n == STATE_ALL) {
        if (ioctl(fd, LEDCMD_GET_STATE, &state) == -1) {
            fprintf(stderr, "STATE ERROR\n");
            return;
        }
        for (led_n = 0 ; led_n < 8; led_n++) {
            printf("Светодиод %d %s\n", led_n,
                   (state & (1 << led_n)) ?
                   "включен" : "выключен");
        }
    } else {
        led.pin = led_n;
        if (ioctl(fd, LEDCMD_GET_LED_STATE, &led) == -1) {
            fprintf(stderr, "LED STATE ERROR\n");
            return;
        }
        printf("Светодиод %d %s\n", led.pin,
               led.value ? "включен" : "выключен");
    }

    close(fd);
}


void led_off(int led_n) {
    int fd;
    led_t led;

    /* open device /dev/leds */
    if ((fd = open(FILENAME, O_RDWR)) == -1) {
        fprintf(stderr, "Cannot open file %s\n", FILENAME);
        return;
    }

    led.pin = led_n;
    /* disable pin */
    led.value = 0;
    if (ioctl(fd, LEDCMD_SET_LED_STATE, &led) == -1) {
        fprintf(stderr, "TRURN OFF ERROR\n");
        return;
    } else {
        if (ioctl(fd, LEDCMD_GET_LED_STATE, &led) == -1) {
            fprintf(stderr, "LED TURN OFF ERROR\n");
            return;
        }
        printf("Светодиод %d %s\n", led.pin, led.value ?
                "включен" : "выключен");
    }
    close(fd);
}

void led_on(int led_n) {
    int fd;
    led_t led;

    fd = open(FILENAME, O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "Cannot open file %s\n", FILENAME);
        return;
    }

    led.pin = led_n;
    /* enable pin */
    led.value = 1;
    if (ioctl(fd, LEDCMD_SET_LED_STATE, &led) == -1) {
        fprintf(stderr, "TURN ON ERROR\n");
        return;
    } else {
        if (ioctl(fd, LEDCMD_GET_LED_STATE, &led) == -1) {
            fprintf(stderr, "TURN ON ERROR!\n");
            return;
        }
        printf("Светодиод %d %s\n", led.pin, led.value ?
                "включен" : "выключен");
    }
    close(fd);
}

int main(int argc, char **argv) {
    char *reset = "reset";
    char *ledst= "ledstate";
    char *on = "on";
    char *off = "off";

    switch (argc) {

    case 2:
        if  (strcmp (argv[1], reset) == 0) {
            led_reset();
        } else if (strcmp (argv[1], ledst) == 0) {
            led_state(STATE_ALL);
        } else {
            print_usage();
        }
        break;

    case 3:
        if  (strcmp (argv[1], ledst) == 0) {
            led_state(atoi(argv[2]));
        } else if (strcmp (argv[1], off) == 0) {
            led_off(atoi(argv[2]));
        } else if (strcmp (argv[1], on) == 0) {
            led_on(atoi(argv[2]));
        }
        break;
    default:
        print_usage();
    }
    return 0;
}

void print_usage(void) {
    puts("Illegal syntax. The usage is: \n"
         "client { reset | ledstate | on | off } [N]\n"
         "\t where { ledstate | on | off } could be followed\n"
         "\t by optional number [N] of particular led");
}
