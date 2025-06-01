/*
 * debug.h
 *
 *  Created on: Feb 23, 2025
 *      Author: Atomic
 */

// All credit to Carmine Noviello for this code
// https://github.com/cnoviello/mastering-stm32/blob/master/nucleo-f030R8/system/include/retarget/retarget.h

#ifndef INC_DEBUG_H_
#define INC_DEBUG_H_

#include "stm32h7xx_hal.h"
#include <sys/stat.h>

void DebugInit(UART_HandleTypeDef *huart);
int _isatty(int fd);
int _write(int fd, char* ptr, int len);
int _close(int fd);
int _lseek(int fd, int ptr, int dir);
int _read(int fd, char* ptr, int len);
int _fstat(int fd, struct stat* st);

#endif /* INC_DEBUG_H_ */
