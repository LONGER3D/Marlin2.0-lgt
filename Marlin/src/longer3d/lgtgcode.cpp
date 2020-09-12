#include "../inc/MarlinConfigPre.h"

#if ENABLED(LGT_LCD_TFT)
#include "../gcode/gcode.h"
#include "lgttouch.h"
#include "lgtstore.h"
#include "lgttftlcd.h"

#define DEBUG_OUT 0
#include "../../core/debug_out.h"

/**
 * @brief start touch calibration and save touch data
 *        in spiflash, or clear touch data in spiflash 
 */  
void GcodeSuite::M995()
{
    if (parser.seen('C'))
        lgtStore.clearTouch();
    else
        lgtTouch.calibrate();
}

/**
 * @brief clear settings in spiflash 
 */  
void GcodeSuite::M2100()
{
   if (parser.seen('C'))
        lgtStore.clearSettings();
}

/**
 * @brief change to MENU
 */
void GcodeSuite::M2101()
{
    if (parser.seen('P')) {
        int16_t pageNum = parser.value_int();
        DEBUG_ECHOPAIR("parse P:", pageNum);
        switch (pageNum) {
        case 0:
            lgtlcdtft.changePageAtOnce(eMENU_MOVE);
            break;
        case 1:
            lgtlcdtft.changePageAtOnce(eMENU_LEVELING);
            break;
        default:
            break;
        }
    }
}

#endif
