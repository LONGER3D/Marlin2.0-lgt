#include "../inc/MarlinConfig.h"

#if ENABLED(LGT_LCD_TFT)
#include "lgttouch.h"
#include "lcddrive/lcdapi.h"
#include "lgttftlanguage.h"
#include "lgtstore.h"

#define DEBUG_LGT_TOUCH
#define DEBUG_OUT ENABLED(DEBUG_LGT_TOUCH)
#include "../../core/debug_out.h"

#define MAX_READ_XY_REPEAT 10u	// max touch read times
#define ERR_RANGE 50u           // touch error range

LgtTouch lgtTouch;

/**
 * read touch x-y value
 * value can't less than 100 or more than 4000
 */
uint8_t LgtTouch::readTouchXY(uint16_t &x,uint16_t &y)
{
	uint16_t xtemp,ytemp;
	xtemp = touch.getInTouch(XPT2046_X);
	if (xtemp < 100 || xtemp > 4000)
		return 0; // failed
	ytemp = touch.getInTouch(XPT2046_Y);	  												   
	if(ytemp < 100 || ytemp > 4000)
		return 0; // failed
	x=xtemp;
	y=ytemp;
	return 1;   // successful
}

/** read x-y ad value two time
 */
uint8_t LgtTouch::readTouchXY2(uint16_t &x, uint16_t &y) 
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
	for (uint8_t i = 0; i < MAX_READ_XY_REPEAT; ++i) {
		if (readTouchXY(x1,y1))
			if (readTouchXY(x2,y2))
				if ((x1 > x2 - ERR_RANGE) && (x2 > x1 - ERR_RANGE) && 
					(y1 > y2 -ERR_RANGE) && (y2 > y1 - ERR_RANGE)) {
						x = (x1 + x2)/2;
						y = (y1 + y2)/2;									
						return 1;
					}						
	}	
	return 0;
}

uint8_t LgtTouch::readTouchPoint(uint16_t &x, uint16_t &y)
{
  if (calib.xCalibration + calib.xOffset == 0) {
    // Not yet set, so use defines as fallback...
    calib.xCalibration = XPT2046_X_CALIBRATION;
    calib.xOffset      = XPT2046_X_OFFSET;
    calib.yCalibration = XPT2046_Y_CALIBRATION;
    calib.yOffset      = XPT2046_Y_OFFSET;
  }

  if (!isTouched())
    return 0;
  uint16_t xAd, yAd;
  if (readTouchXY2(xAd, yAd)) {
    x = uint16_t((uint32_t(xAd) * calib.xCalibration) >> 16) + calib.xOffset;
    y = uint16_t((uint32_t(yAd) * calib.yCalibration) >> 16) + calib.yOffset;
  } else {
    DEBUG_ECHOLN("read touch failed");
    x = y = 0;
    return 0;
  }
  if (!isTouched()) // recheck if touched
    return 0;
  return 1;
}

uint8_t LgtTouch::calibrate()
{
  uint16_t color = YELLOW;
  uint16_t bgColor = BLACK;
  uint16_t length;
  // uint32_t i;
  uint16_t x[4] = {0,0,0,0};
  uint16_t y[4] = {0,0,0,0};
  char text[41];
  SERIAL_ECHOLN("start calibration");

  lgtlcd.setColor(color);
  lgtlcd.setBgColor(bgColor);

  for (uint8_t i = 0; i < 4;) {
    lgtlcd.clear(bgColor);

    /**
     * Test coordinates and colors inversion.
     * Draw RED and GREEN squares in top left area of the screen.
     */
    lgtlcd.setWindow(40, 20, 99, 69);

    length = sprintf(text, TXT_TFT_CONTROLLER_ID, lgtlcd.lcdId());
    // center horizontal alignment, x = (320 - len * 8) / 2
    lgtlcd.print(160 - length * 4, 48, text);
    // length = sprintf(text, TXT_TFT_CONTROLLER, controller);
    // lgtlcd.print(160 - length * 4, 66, text);

    length = sprintf(text, TXT_TOUCH_CALIBRATION);
    lgtlcd.print(92, 88, text);

    switch (i) {
      case 0:
        lgtlcd.drawCross( 20 , 20, 0xFFFF);
        length = sprintf(text, TXT_TOP_LEFT);
        break;
      case 1:
        lgtlcd.drawCross( 20, 219, 0xFFFF);
        length = sprintf(text, TXT_BOTTOM_LEFT);
        break;
      case 2:
        lgtlcd.drawCross(299,  20, 0xFFFF);
        length = sprintf(text, TXT_TOP_RIGHT);
        break;
      case 3:
        lgtlcd.drawCross(299, 219, 0xFFFF);
        length = sprintf(text, TXT_BOTTOM_RIGHT);
        break;
    }
    lgtlcd.print(160 - length * 4, 108, text);

    // wait for touch then read touch
    // waitForTouch(x[i], y[i]);
    while (1) {
      if ((isTouched()) && readTouchXY2(x[i], y[i]))
        break;
    }

    DEBUG_ECHOPAIR("\ntouched i:", i);
    DEBUG_ECHOLNPAIR(" x:", x[i]);
    DEBUG_ECHOLNPAIR(" y:", y[i]);
    if ((x[i] < 409 || x[i] > 1637) && (y[i] < 409 || y[i] > 1637)) {

      switch (i) {
        case 0: // Top Left
          i++;
          waitForRelease();
          delay(300);
          continue;
        case 1: // Bottom Left
          if (((x[0] < 409 && x[1] < 409) || (x[0] > 1637 && x[1] > 1637)) && ((y[0] < 409 && y[1] > 1637) || (y[0] > 1637 && y[1] < 409))) {
            i++;
            waitForRelease();
            delay(300);
            continue;
          }
          break;
        case 2: // Top Right
          if (((x[0] < 409 && x[2] > 1637) || (x[0] > 1637 && x[2] < 409)) && ((y[0] < 409 && y[2] < 409) || (y[0] > 1637 && y[2] > 1637))) {
            i++;
            waitForRelease();
            delay(300);
            continue;
          }
          break;
        case 3: // Bottom Right
          if (((x[0] < 409 && x[3] > 1637) || (x[0] > 1637 && x[3] < 409)) && ((y[0] < 409 && y[3] > 1637) || (y[0] > 1637 && y[3] < 409))) {
            i++;
            waitForRelease();
            delay(300);
            continue;
          }
          break;
        }
    }
    delay(1500);
    lgtlcd.clear(RED);
    waitForRelease();
    delay(500);
  }

  // calulate touch coefficient
  // 36569088L == ((int32_t)(299 - 20)) << 17
  calib.xCalibration = (int16_t)(36569088L / ((int32_t)x[3] + (int32_t)x[2] - (int32_t)x[1] - (int32_t)x[0]));
  // 26083328L == ((int32_t)(219 - 20)) << 17
  calib.yCalibration = (int16_t)(26083328L / ((int32_t)y[3] - (int32_t)y[2] + (int32_t)y[1] - (int32_t)y[0]));
  calib.xOffset = (int16_t)(20 - ((((int32_t)(x[0] + x[1])) * (int32_t)calib.xCalibration) >> 17));
  calib.yOffset = (int16_t)(20 - ((((int32_t)(y[0] + y[2])) * (int32_t)calib.yCalibration) >> 17));

  // print result to lcd

  lgtlcd.clear(bgColor);
  length = sprintf(text, TXT_TFT_CONTROLLER_ID, lgtlcd.lcdId());
  lgtlcd.print(160 - length * 4, 48, text);
  // length = sprintf(text,TXT_TFT_CONTROLLER, controller);
  // lgtlcd.print(160 - length * 4, 66, text);
  lgtlcd.setColor(GREEN);
  length = sprintf(text, TXT_CALI_COMPLETED);
  lgtlcd.print(160 - length * 4, 88, text);
  lgtlcd.setColor(YELLOW);

  sprintf(text, TXT_X_CALIBRATION);
  lgtlcd.print(76, 108, text);
  sprintf(text, "%6d", calib.xCalibration);
  lgtlcd.m_color = calib.xCalibration >= 0 ? GREEN : RED;
  lgtlcd.print(196, 108, text);
  lgtlcd.m_color = YELLOW;

  sprintf(text, TXT_Y_CALIBRATION);
  lgtlcd.print(76, 124, text);
  sprintf(text, "%6d", calib.yCalibration);
  lgtlcd.m_color = calib.yCalibration >= 0 ? GREEN : RED;
  lgtlcd.print(196, 124, text);
  lgtlcd.m_color = YELLOW;

  sprintf(text, TXT_X_OFFSET);
  lgtlcd.print(76, 140, text);
  sprintf(text, "%6d", calib.xOffset);
  lgtlcd.m_color = calib.xOffset >= 0 ? GREEN : RED;
  lgtlcd.print(196, 140, text);
  lgtlcd.m_color = YELLOW;

  sprintf(text, TXT_Y_OFFSET);
  lgtlcd.print(76, 156, text);
  sprintf(text, "%6d", calib.yOffset);
  lgtlcd.m_color = calib.yOffset >= 0 ? GREEN : RED;
  lgtlcd.print(196, 156, text);
  lgtlcd.m_color = GREEN;
  length = sprintf(text, TXT_PROMPT_INFO1);
  lgtlcd.print(160 - length * 4, 180, text);  // center alignment
  // length = sprintf(text, TXT_PROMPT_INFO2);
  // lgtlcd.print(160 - length * 4, 198, text);
  lgtlcd.m_color = WHITE;

  // print result to serial
  MYSERIAL0.print("xCalibration:");
  MYSERIAL0.print(calib.xCalibration);
  MYSERIAL0.println();
  MYSERIAL0.print("yCalibration:");
  MYSERIAL0.print(calib.yCalibration);
  MYSERIAL0.println();
  MYSERIAL0.print("xOffset:");
  MYSERIAL0.print(calib.xOffset);
  MYSERIAL0.println();
  MYSERIAL0.print("yOffset:");
  MYSERIAL0.print(calib.yOffset);

  x[0] = (uint16_t)((((int32_t)x[0] * (int32_t)calib.xCalibration) >> 16) + calib.xOffset);
  x[1] = (uint16_t)((((int32_t)x[1] * (int32_t)calib.xCalibration) >> 16) + calib.xOffset);
  x[2] = (uint16_t)((((int32_t)x[2] * (int32_t)calib.xCalibration) >> 16) + calib.xOffset);
  x[3] = (uint16_t)((((int32_t)x[3] * (int32_t)calib.xCalibration) >> 16) + calib.xOffset);
  y[0] = (uint16_t)((((int32_t)y[0] * (int32_t)calib.yCalibration) >> 16) + calib.yOffset);
  y[1] = (uint16_t)((((int32_t)y[1] * (int32_t)calib.yCalibration) >> 16) + calib.yOffset);
  y[2] = (uint16_t)((((int32_t)y[2] * (int32_t)calib.yCalibration) >> 16) + calib.yOffset);
  y[3] = (uint16_t)((((int32_t)y[3] * (int32_t)calib.yCalibration) >> 16) + calib.yOffset);

  MYSERIAL0.println("\nCalibrated coordinates:");
  MYSERIAL0.print("X: "); MYSERIAL0.print(x[0]); MYSERIAL0.print("   Y: "); MYSERIAL0.println(y[0]);
  MYSERIAL0.print("X: "); MYSERIAL0.print(x[1]); MYSERIAL0.print("   Y: "); MYSERIAL0.println(y[1]);
  MYSERIAL0.print("X: "); MYSERIAL0.print(x[2]); MYSERIAL0.print("   Y: "); MYSERIAL0.println(y[2]);
  MYSERIAL0.print("X: "); MYSERIAL0.print(x[3]); MYSERIAL0.print("   Y: "); MYSERIAL0.println(y[3]);
  MYSERIAL0.flush();
 
  // save data
  lgtStore.saveTouch();
  // wait for touch
  while (!isTouched()) { /* nada */ }
  lgtlcd.setColor(BLACK);
  lgtlcd.setBgColor(WHITE);

 return 1;
}

void LgtTouch::resetCalibration()
{
    calib.xCalibration = XPT2046_X_CALIBRATION;
    calib.yCalibration = XPT2046_Y_CALIBRATION;
    calib.xOffset      = XPT2046_X_OFFSET;
    calib.yOffset      = XPT2046_Y_OFFSET; 
}


#endif