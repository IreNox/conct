#pragma once

/*
   tiny_gpio.h
   2018-06-19
   Public Domain
*/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* gpio modes. */

#define PI_INPUT  0
#define PI_OUTPUT 1
#define PI_ALT0   4
#define PI_ALT1   5
#define PI_ALT2   6
#define PI_ALT3   7
#define PI_ALT4   3
#define PI_ALT5   2

void gpioSetMode(unsigned gpio, unsigned mode);
int gpioGetMode(unsigned gpio);

/* Values for pull-ups/downs off, pull-down and pull-up. */

#define PI_PUD_OFF  0
#define PI_PUD_DOWN 1
#define PI_PUD_UP   2

void gpioSetPullUpDown(unsigned gpio, unsigned pud);

int gpioRead(unsigned gpio);
void gpioWrite(unsigned gpio, unsigned level);
void gpioTrigger(unsigned gpio, unsigned pulseLen, unsigned level);

/* Bit (1<<x) will be set if gpio x is high. */

uint32_t gpioReadBank1(void);
uint32_t gpioReadBank2(void);

/* To clear gpio x bit or in (1<<x). */

void gpioClearBank1(uint32_t bits);
void gpioClearBank2(uint32_t bits);

/* To set gpio x bit or in (1<<x). */

void gpioSetBank1(uint32_t bits);
void gpioSetBank2(uint32_t bits);

int gpioInitialise(void);

#ifdef __cplusplus
}
#endif
