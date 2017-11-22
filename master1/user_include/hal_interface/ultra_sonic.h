#ifndef ULTRA_SONIC_H_
#define ULTRA_SONIC_H_	1

#include "hal_common_includes.h"

typedef enum {
	ultra_flag
} flag;

void ultra_init(void);
static void ultra_gpio_init(void);

#endif