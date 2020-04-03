/*
 * debug.h
 *
 *  Created on: 2020/04/03
 *      Author: Akinori
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "Std_Types.h"
#include <stdarg.h>

#define DEBUG_HIGH 1

void DEBUG(uint8 level, const char *fmt, ...);

#endif /* DEBUG_H_ */
