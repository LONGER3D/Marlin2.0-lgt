#include "../inc/MarlinConfig.h"

#if ENABLED(LGT_LCD_TFT)
#include "lgttftlcd.h"
#include "lcddrive/lcdapi.h"
#include "../feature/touch/xpt2046.h"

#define DEBUG_LGTLCDTFT
#define DEBUG_OUT ENABLED(DEBUG_LGTLCDTFT)
#include "../../core/debug_out.h"

extern uint8_t init_Lgt_Tft_Lcd();

LgtLcdTft lgtlcdtft;

LgtLcdTft::LgtLcdTft()
{

}

void LgtLcdTft::init()
{
    lgtlcd.init();

}

void LgtLcdTft::loop()
{
    #define TOUCH_DELAY 150u // millsecond
    static millis_t nextTouchReadTime = 0;
    static bool touched = false;
    static uint16_t touchX = 0, touchY = 0;
    if (touch.isTouched()) {
        const millis_t time = millis();
        if (time > nextTouchReadTime) {
            nextTouchReadTime = time + TOUCH_DELAY;
            if (touch.isTouched()) {    // truely touched
                touched = true;
                // DEBUG_ECHOLNPGM("lcd: touched");
                touch.getTouchPoint(touchX, touchY);
                // DEBUG_ECHOLNPAIR("lcd-x: ", x);
                // DEBUG_ECHOLNPAIR("lcd-y: ", y);                

            }
        }
    } else if (touched) {  // released
        touched = false;
        DEBUG_ECHOLNPGM("lcd: released");
        DEBUG_ECHOLNPAIR("lcd-x: ", touchX);
        DEBUG_ECHOLNPAIR("lcd-y: ", touchY);     
    } else {    // idle
    
    }
}

#endif