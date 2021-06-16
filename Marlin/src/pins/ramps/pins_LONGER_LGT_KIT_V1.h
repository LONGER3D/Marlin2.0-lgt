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

#include "pins_RAMPS.h"

#undef SERVO0_PIN
#undef Z_MIN_PIN
#undef FIL_RUNOUT_PIN
#ifdef SD_DETECT_PIN
  #undef SD_DETECT_PIN
#endif

#ifdef KILL_PIN
  #undef KILL_PIN
#endif

#ifdef CR10_STOCKDISPLAY
  #undef LCD_PINS_RS
  #undef LCD_PINS_ENABLE
  #undef LCD_PINS_D4
  #undef BTN_EN1
  #undef BTN_EN2

  #define LCD_PINS_RS          5
  #define LCD_PINS_ENABLE      4
  #define LCD_PINS_D4          6
  #define BTN_EN1              18
  #define BTN_EN2              19

#endif

#ifdef BEEPER_PIN
  #undef BEEPER_PIN
  #define BEEPER_PIN           11
#endif

#ifdef BTN_ENC
  #undef BTN_ENC
  #define BTN_ENC              15
#endif

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
  #define Z_MIN_PIN          11
#elif defined(LK4_PRO)
  #define Z_MIN_PIN          35
#endif

#define SD_DETECT_PIN   49
#define FIL_RUNOUT_PIN  2





