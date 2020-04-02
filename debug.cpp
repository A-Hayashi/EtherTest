/*
 * debug.c
 *
 *  Created on: 2020/04/03
 *      Author: Akinori
 */
#include "debug.h"

void DEBUG(uint8 level, const char *fmt, ...)
{
  char buf[128];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, 128, fmt, args);
  va_end(args);
  Serial.print(buf);
}
