#pragma once

#include "../feature/touch/xpt2046.h"

class LgtTouch
{
private:
    static int16_t xCalibration;
    static int16_t yCalibration;
    static int16_t xOffset;
    static int16_t yOffset;
private:
    uint8_t readTouchXY(uint16_t &x,uint16_t &y);
    uint8_t readTouchXY2(uint16_t &x,uint16_t &y);
public:
    inline bool isTouched() { return touch.isTouched(); }
    inline void waitForRelease() { touch.waitForRelease();}
    // inline void waitForTouch(uint16_t &x, uint16_t &y) { touch.waitForTouch(x, y); }
    uint8_t readTouchPoint(uint16_t &x, uint16_t &y);
    uint8_t calibrate();
};

extern LgtTouch lgtTouch;


