#pragma once

#include "../../inc/MarlinConfig.h"

#if ENABLED(LGT_LCD_TFT)
#define LCD_Delay(ms) _delay_ms(ms)

void LCD_IO_Init(uint8_t cs, uint8_t rs);
void LCD_IO_WriteData(uint16_t RegValue);
void LCD_IO_WriteReg(uint16_t Reg);
uint16_t LCD_IO_ReadData(uint16_t RegValue);
uint32_t LCD_IO_ReadData(uint16_t RegValue, uint8_t ReadSize);

#ifdef LCD_USE_DMA_FSMC
  void LCD_IO_WriteMultiple(uint16_t data, uint32_t count);
  void LCD_IO_WriteSequence(uint16_t *data, uint16_t length);
  void LCD_IO_WriteSequence_Async(uint16_t *data, uint16_t length);
  void LCD_IO_WaitSequence_Async();
#endif

#endif