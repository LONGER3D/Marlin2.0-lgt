#include "../inc/MarlinConfig.h"
#include "lgttftlcd.h"

extern uint8_t init_Lgt_Tft_Lcd();

#if ENABLED(LGT_LCD_TFT)

LgtLcdTft lgtlcdtft;

void LgtLcdTft::begin()
{
    init_Lgt_Tft_Lcd();

}

#endif