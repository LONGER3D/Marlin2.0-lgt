#pragma once

#include "../feature/touch/xpt2046.h"

// init value in load touch
struct TouchCalibration
{
    char version[4];   // Vxx/0
    int16_t xCalibration;
    int16_t yCalibration;
    int16_t xOffset;
    int16_t yOffset;
};

class LgtTouch
{
private:
    TouchCalibration calib;
private:
    uint8_t readTouchXY(uint16_t &x,uint16_t &y);
    uint8_t readTouchXY2(uint16_t &x,uint16_t &y);
public:
    inline bool isTouched() { return touch.isTouched(); }
    inline void waitForRelease() { touch.waitForRelease();}
    // inline void waitForTouch(uint16_t &x, uint16_t &y) { touch.waitForTouch(x, y); }
    uint8_t readTouchPoint(uint16_t &x, uint16_t &y);
    uint8_t calibrate(bool needKill=true);
    TouchCalibration &calibrationData() { return calib; }
    void resetCalibration();
};

extern LgtTouch lgtTouch;


