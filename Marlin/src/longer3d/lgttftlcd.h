#pragma once

// #include "../inc/MarlinConfigPre.h"

#if ENABLED(LGT_LCD_TFT)
class LgtLcdTft {

public:
    LgtLcdTft();
    void init();
    void loop();
};

extern LgtLcdTft lgtlcdtft; 

#endif
