#pragma once

#include <stdint.h>

/**
  * @brief  ILI9341 Registers
  */

#define ILI9341_SWRESET            0x01   /* Software Reset */
#define ILI9341_LCD_ID             0xD3
#define ILI9341_SLEEP_IN           0x10
#define ILI9341_SLEEP_OUT          0x11
#define ILI9341_PARTIAL_DISPLAY    0x12
#define ILI9341_DISPLAY_INVERSION  0x21
#define ILI9341_DISPLAY_OFF        0x28
#define ILI9341_DISPLAY_ON         0x29
#define ILI9341_WRITE_RAM          0x2C
#define ILI9341_READ_RAM           0x2E
#define ILI9341_CASET              0x2A
#define ILI9341_RASET              0x2B
#define ILI9341_VSCRDEF            0x33 /* Vertical Scroll Definition */
#define ILI9341_VSCSAD             0x37 /* Vertical Scroll Start Address of RAM */
#define ILI9341_TEARING_EFFECT     0x35
#define ILI9341_NORMAL_DISPLAY     0x36
#define ILI9341_IDLE_MODE_OFF      0x38
#define ILI9341_IDLE_MODE_ON       0x39
#define ILI9341_COLOR_MODE         0x3A

#define ILI9341_ID4                0xD3  // real ID register for 9341

#define ILI9341_BLKING_PORCH_CTRL  0xB5

#define ILI9341_VCOM_CTRL1          0xC5
#define ILI9341_VCOM_CTRL2          0xC7

#define ILI9341_FR_CTRL             0xB1

#define ILI9341_POWER_CTRL1         0xC0
#define ILI9341_POWER_CTRL2         0xC1

#define ILI9341_ID                  0x9341


void ILI9341_Init(void);
void ILI9341_DisplayOn(void);
void ILI9341_WriteRam(void);
void ILI9341_SetCursor(uint16_t Xpos, uint16_t Ypos);
void ILI9341_SetWindow(uint16_t Xmin, uint16_t Ymin, uint16_t XMax = 319, uint16_t Ymax = 239);
