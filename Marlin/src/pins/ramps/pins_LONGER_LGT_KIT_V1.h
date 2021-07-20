/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

/**
 * LONGER LKx Pro board (mega2560) pin assignments
 */

#if HOTENDS > 1 || E_STEPPERS > 1
  #error " LONGER LKx Pro board (mega2560) only supports 1 hotend / E-stepper. Comment out this line to continue."
#endif

#define BOARD_INFO_NAME "LONGER LGT KIT V1.0"

// #define LK1_PRO
#define LK4_PRO

#if defined(LK1_PRO)

#elif defined(LK4_PRO)

#endif
//
// Servos
//
#if defined(LK1_PRO)
#define SERVO0_PIN          11
#elif defined(LK4_PRO)
#define SERVO0_PIN          7
#endif

//
// Limit Switches
//
#if defined(LK1_PRO)
  #define Z_STOP_PIN          11
#elif defined(LK4_PRO)
  #define Z_STOP_PIN          35
#endif

// #define SD_DETECT_PIN   49
#define FIL_RUNOUT_PIN  2

#include "pins_RAMPS.h"


