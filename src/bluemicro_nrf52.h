// SPDX-FileCopyrightText: 2022 BlueMicro_Engine_Arduino_Library contributors (https://github.com/jpconstantineau/BlueMicro_Engine_Arduino_Library/graphs/contributors)
//
// SPDX-License-Identifier: BSD-3-Clause

#ifndef BLUEMICRO_NRF52_H
#define BLUEMICRO_NRF52_H
#include <Arduino.h>

void setupGpio(void);

void setupWDT(void);
void updateWDT(void);

void reboot(void);

uint32_t analogReadVDD(void);


#endif