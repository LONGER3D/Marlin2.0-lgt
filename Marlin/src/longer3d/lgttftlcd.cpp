#include "../inc/MarlinConfig.h"

#if ENABLED(LGT_LCD_TFT)
#include "lgttftlcd.h"
#include "lcddrive/lcdapi.h"
#include "../feature/touch/xpt2046.h"
#include "w25qxx.h"

#define DEBUG_LGTLCDTFT
#define DEBUG_OUT ENABLED(DEBUG_LGTLCDTFT)
#include "../../core/debug_out.h"

LgtLcdTft lgtlcdtft;

LgtLcdTft::LgtLcdTft()
{

}


// void LgtLcdTft::displayStartUpLogo(void)
// {
//   lcd.clear(White);
//   #if defined(U30) || defined(U20) || defined(U20_PLUS) 
//   	displayImage(60, 95, IMG_ADDR_STARTUP_LOGO_0);
//   #elif defined(LK1_PLUS) ||  defined(LK1) || defined(LK2) || defined(LK4)  
// 	displayImage(45, 100, 991586);
//   #endif
// }

void LgtLcdTft::init()
{
    lgtlcd.init();
    lgtlcd.showImage(45, 100, 991586);

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
                touch.readTouchPoint(touchX, touchY);
                // DEBUG_ECHOLNPAIR("lcd-x: ", x);
                // DEBUG_ECHOLNPAIR("lcd-y: ", y);                

            }
        }
    } else if (touched) {  // released
        touched = false;
        DEBUG_ECHOLNPGM("touch: released");
        DEBUG_ECHOLNPAIR("touch-x: ", touchX);
        DEBUG_ECHOLNPAIR("touch-y: ", touchY);     
    } else {    // idle
    
    }
}

#endif