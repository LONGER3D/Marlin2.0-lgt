#include "../inc/MarlinConfigPre.h"

#if ENABLED(LGT_LCD_TFT)
#include "../gcode/gcode.h"
#include "lgttouch.h"
#include "lgtstore.h"

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


#endif
