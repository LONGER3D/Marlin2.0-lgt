#include "../inc/MarlinConfig.h"

#if ENABLED(LGT_LCD_TFT)
#include "lgttftlcd.h"
#include "lcddrive/lcdapi.h"

extern uint8_t init_Lgt_Tft_Lcd();

LgtLcdTft lgtlcdtft;

LgtLcdTft::LgtLcdTft()
{

}

void LgtLcdTft::begin()
{
    lcd.begin();
}

#endif