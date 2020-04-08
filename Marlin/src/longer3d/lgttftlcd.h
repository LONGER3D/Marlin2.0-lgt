#pragma once

// #include "../inc/MarlinConfigPre.h"

#if ENABLED(LGT_LCD_TFT)
class LgtLcdTft {
public:
    LgtLcdTft();
    void begin();

};

extern LgtLcdTft lgtlcdtft; 

#endif
