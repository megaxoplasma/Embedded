#ifndef ULTRA_HAL_H_
#define ULTRA_HAL_H_	1

#include "hal_common_includes.h"

typedef enum {
	ultra_flag
} flag;

void ultra_init(void);
int ultra_change();

#endif