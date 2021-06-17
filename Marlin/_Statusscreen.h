/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
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

/**
 * Custom Status Screen bitmap
 *
 * Place this file in the root with your configuration files
 * and enable CUSTOM_STATUS_SCREEN_IMAGE in Configuration.h.
 *
 * Use the Marlin Bitmap Converter to make your own:
 * http://marlinfw.org/tools/u8glib/converter.html
 */
#pragma once

//
// Status Screen Logo bitmap
//
#define STATUS_LOGO_X            2
#define STATUS_LOGO_Y            5
#define STATUS_LOGO_WIDTH        40

const unsigned char status_logo_bmp[] PROGMEM = {
  B00000000,B00000000,B00000000,B00000000,B00000010,
  B00000000,B00000000,B00000000,B00000000,B00001100,
  B00000001,B11100000,B11111100,B00111000,B11111000,
  B00000011,B00000001,B10011000,B11110000,B10000000,
  B00000110,B00000011,B00011001,B10000001,B10000000,
  B00000110,B00000011,B00011011,B00000001,B00000000,
  B00001110,B00000011,B00011100,B00000011,B11110000,
  B00001100,B00000011,B00111100,B00000010,B01110000,
  B00001100,B00000000,B00111100,B00000000,B00111000,
  B00011100,B00010000,B00111110,B00000000,B00110000,
  B00011000,B00010000,B01100110,B00000000,B00110000,
  B00010000,B00110000,B01100111,B00000000,B00100000,
  B11111111,B11100000,B11000011,B00000100,B11000000,
  B00011111,B11100111,B11100011,B00001111,B10000000,
  B00000000,B00000000,B00000011,B10000000,B00000000,
  B00000000,B00000000,B00000001,B11001000,B00000000,
  B00000000,B00000000,B00000000,B11111000,B00000000
};


//
// Use default bitmaps
//
#define STATUS_HOTEND_ANIM
#define STATUS_BED_ANIM
#define STATUS_HEATERS_XSPACE   20
#if HOTENDS < 2
  #define STATUS_HEATERS_X      48
  #define STATUS_BED_X          72
#else
  #define STATUS_HEATERS_X      40
  #define STATUS_BED_X          80
#endif
